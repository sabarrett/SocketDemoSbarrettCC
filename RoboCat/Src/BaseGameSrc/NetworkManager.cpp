#include "NetworkManager.h"
#include <time.h>
namespace
{
	const float kTimeBetweenHellos = 1.f;
	const float kStartDelay = 0.0f;
	const int	kSubTurnsPerTurn = 3;
	const int	kMaxPlayerCount = 4;
}
NetworkManager::NetworkManager() :
	mBytesSentThisFrame(0),
	mDropPacketChance(0.1f),
	mSimulatedLatency(0.5f),
	mBytesReceivedPerSecond(WeightedTimedMovingAverage(1.f)),
	mBytesSentPerSecond(WeightedTimedMovingAverage(1.f)),
	mPlayerId(0),
	mNewNetworkId(1),
	mIsMasterPeer(false),
	mPlayerCount(0),
	mHighestPlayerId(0),
	mTimeOfLastHello(0.0f),
	mTimeToStart(-1.0f),
	mState(NetworkManagerState::NMS_Unitialized),
	mNextOutgoingSequenceNumber(0),
	mNextExpectedSequenceNumber(0),
	mDispatchedPacketCount(0)
{
	mPendingAcks.clear();
}

NetworkManager::~NetworkManager()
{
	TransmissionData* temp;
	int size = mInFlightPackets.size();
	for (int i = 0; i < size; i++)
	{
		temp = mInFlightPackets.begin()->second;
		mInFlightPackets.erase(mInFlightPackets.begin());
		delete(temp);
		temp = nullptr;
	}
}

bool NetworkManager::InitAsMasterPeer(uint16_t inPort, const string& inName)
{
	if (!InitSocket(inPort))
	{
		return false;
	}
	//master peer at init means guaranteed first player id
	mPlayerId = 1;
	mHighestPlayerId = mPlayerId;
	mIsMasterPeer = true;
	mPlayerCount = 1;

	//in lobby cause we don't need to ask the master peer (since we are the master peer)
	mState = NetworkManagerState::NMS_Lobby;

	//add myself to the player name map
	mName = inName;
	mPlayerNameMap.emplace(mPlayerId, mName);
	std::cout << "Init as Master peer" << std::endl;
	return true;
}

bool NetworkManager::InitAsPeer(const SocketAddress& inMPAddress, const string& inName)
{
	if (!InitSocket(0))
	{
		return false;
	}

	mMasterPeerAddr = inMPAddress;

	//we're going to have to ask the master peer
	mState = NetworkManagerState::NMS_Hello;

	//set my name
	mName = inName;
	//don't know my player id, so can't add myself to the name map yet
	std::cout << "Init as peer" << std::endl;
	return true;
}

bool NetworkManager::InitSocket(uint16_t inPort)
{
	mSocket = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	SocketAddress ownAddress(INADDR_ANY, inPort);
	mSocket->Bind(ownAddress);
	std::cout << "Initializing NetworkManager at port " << inPort << std::endl;
	

	//did we bind okay?
	if (mSocket == nullptr)
	{
		return false;
	}

	if (mSocket->SetNonBlockingMode(true) != NO_ERROR)
	{
		return false;
	}

	return true;
}

void NetworkManager::ProcessIncomingPackets()
{
	ReadIncomingPacketsIntoQueue();

	ProcessQueuedPackets();

	UpdateBytesSentLastFrame();

}

void NetworkManager::SendOutgoingPackets()
{
	switch (mState)
	{
	case NetworkManagerState::NMS_Hello:
		UpdateSayingHello();
		break;
	case NetworkManagerState::NMS_Starting:
		//std::cout << "Starting" << std::endl;
		UpdateStarting();
		break;
	case NetworkManagerState::NMS_Playing:
		//std::cout << "Playing" << std::endl;
		UpdateSendActionPacket();
		break;
	default:
		break;
	}
}

void NetworkManager::UpdateSayingHello(bool inForce)
{
	float time = Timing::sInstance.GetTimef();

	if (inForce || time > mTimeOfLastHello + kTimeBetweenHellos)
	{
		SendHelloPacket();
		mTimeOfLastHello = time;
	}
}

void NetworkManager::SendHelloPacket()
{
	OutputMemoryBitStream helloPacket;

	TransmissionDataHello* newData = new TransmissionDataHello(mName);
	newData->mPacketType = kHelloCC;
	AddInFlightPacket(newData, helloPacket);
	helloPacket.Write(kHelloCC);
	helloPacket.Write(mName);
	std::cout << "Saying hello" << std::endl;

	WritePendingAcks(helloPacket);

	SendPacket(helloPacket, mMasterPeerAddr);
}

void NetworkManager::UpdateStarting()
{
	mTimeToStart -= Timing::sInstance.GetDeltaTime();
	if (mTimeToStart <= 0.0f)
	{
		EnterPlayingState();
	}
}

void NetworkManager::UpdateSendActionPacket()
{
		//we need to send a turn packet to all of our peers
		OutputMemoryBitStream packet;


		TransmissionDataUpdate* newData = new TransmissionDataUpdate(mPlayerId);
		newData->mPacketType = kUpdateCC;
		AddInFlightPacket(newData, packet);

		//std::cout << newData->sequenceID << " Update" << std::endl;


		packet.Write(kUpdateCC);
		packet.Write(mPlayerId);
		uint32_t size = mActionVec.size();
		packet.Write(size);
		//std::cout << "Num actions sent: " << size << std::endl;
		if (size > 0)
		{
			for (uint32_t i = 0; i < size; i++)
			{
				mActionVec[i].Write(packet);
				newData->mData.push_back(mActionVec[i]);
			}
		}

			WritePendingAcks(packet);

			for (auto& iter : mPlayerToSocketMap)
			{
				SendPacket(packet, iter.second);
			}
		
		mActionVec.clear();
}

void NetworkManager::ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
	int sequenceID;
	inInputStream.Read(sequenceID);
	ProcessSequenceNum(sequenceID);
	//std::cout << "Recieved packet #" << sequenceID << std::endl;
	switch (mState)
	{
	case NetworkManagerState::NMS_Hello:
		ProcessPacketsHello(inInputStream, inFromAddress);
		break;
	case NetworkManagerState::NMS_Lobby:
		ProcessPacketsLobby(inInputStream, inFromAddress);
		break;
	case NetworkManagerState::NMS_Playing:
		ProcessPacketsPlaying(inInputStream, inFromAddress);
		break;
	case NetworkManagerState::NMS_Delay:
		ProcessPacketsDelay(inInputStream, inFromAddress);
		break;
	default:
		break;
	}
	ProcessNewAcks(inInputStream);
}

void NetworkManager::ProcessPacketsHello(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
	UNREFERENCED_PARAMETER(inFromAddress);
	//only time we're in hello state is when we are not the master peer and trying to join
	uint32_t	packetType;
	inInputStream.Read(packetType);

	switch (packetType)
	{
	case kNotMasterPeerCC:
		HandleNotMPPacket(inInputStream);
		break;
	case kWelcomeCC:
		std::cout << "Handling welcome" << std::endl;
		HandleWelcomePacket(inInputStream);
		break;
	case kNotJoinableCC:
	default:
		//couldn't join or something crazy happened, goodbye cruel world
		//LOG("Failed to join game, exiting...");
		//Engine::sInstance->SetShouldKeepRunning(false);
		break;
	}
}

void NetworkManager::HandleNotMPPacket(InputMemoryBitStream& inInputStream)
{
	//this will have the correct master peer address
	mMasterPeerAddr.Read(inInputStream);
	//just force send this immediately
	UpdateSayingHello(true);
}

void NetworkManager::HandleWelcomePacket(InputMemoryBitStream& inInputStream)
{
	//first is my player id
	int playerId;
	inInputStream.Read(playerId);
	UpdateHighestPlayerId(playerId);
	mPlayerId = playerId;

	//add me to the name map
	mPlayerNameMap.emplace(mPlayerId, mName);

	//now the player id for the master peer
	//add entries for the master peer
	inInputStream.Read(playerId);
	UpdateHighestPlayerId(playerId);
	mPlayerToSocketMap.emplace(playerId, mMasterPeerAddr);
	mSocketToPlayerMap.emplace(mMasterPeerAddr, playerId);

	//now remaining players
	inInputStream.Read(mPlayerCount);
	SocketAddress socketAddr;
	// there's actually count - 1 entries because the master peer won't have an entry for themselves
	for (int i = 0; i < mPlayerCount - 1; ++i)
	{
		inInputStream.Read(playerId);
		UpdateHighestPlayerId(playerId);

		socketAddr.Read(inInputStream);

		mPlayerToSocketMap.emplace(playerId, socketAddr);
		mSocketToPlayerMap.emplace(socketAddr, playerId);
	}

	//now player names
	std::string name;
	for (int i = 0; i < mPlayerCount; ++i)
	{
		inInputStream.Read(playerId);
		inInputStream.Read(name);
		mPlayerNameMap.emplace(playerId, name);
	}

	//now add 1 to the player count cause I've joined
	//(the master peer sends me the old list before adding me)
	mPlayerCount++;

	//now we need to send out an intro packet to every peer in the socket map
	OutputMemoryBitStream outPacket;

	TransmissionDataIntro* newData = new TransmissionDataIntro(mName, mPlayerId);
	newData->mPacketType = kIntroCC;
	AddInFlightPacket(newData, outPacket);
	outPacket.Write(kIntroCC);
	outPacket.Write(mPlayerId);
	outPacket.Write(mName);

	
	WritePendingAcks(outPacket);

	for (auto& iter : mPlayerToSocketMap)
	{
		SendPacket(outPacket, iter.second);
	}

	//I've been welcomed, so I'm in the lobby now
	mState = NetworkManagerState::NMS_Lobby;
}

void NetworkManager::ProcessPacketsLobby(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
	//could be someone saying hello, an introduction, or a start packet
	uint32_t	packetType;
	inInputStream.Read(packetType);

	switch (packetType)
	{
	case kHelloCC:
		HandleHelloPacket(inInputStream, inFromAddress);
		break;
	case kIntroCC:
		HandleIntroPacket(inInputStream, inFromAddress);
		break;
	case kStartCC:
		HandleStartPacket(inInputStream, inFromAddress);
		break;
	default:
		LOG("Unexpected packet received in Lobby state. Ignoring.");
		break;
	}
}

void NetworkManager::HandleHelloPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
	//for now, if I already know of this player, throw away the packet
	//this doesn't work if there's packet loss
	if (mSocketToPlayerMap.find(inFromAddress) != mSocketToPlayerMap.end())
	{
		return;
	}

	if (mPlayerCount >= kMaxPlayerCount)
	{
		//sorry, can't join if full
		OutputMemoryBitStream outPacket;
		TransmissionData* newData = new TransmissionData;
		AddInFlightPacket(newData, outPacket);
		newData->mPacketType = kNotJoinableCC;
		outPacket.Write(kNotJoinableCC);
		WritePendingAcks(outPacket);
		SendPacket(outPacket, inFromAddress);
		return;
	}

	if (mIsMasterPeer)
	{
		//it'll only contain the new player's name
		string name;
		inInputStream.Read(name);
		OutputMemoryBitStream outputStream;
		TransmissionDataWelcome* newPacket = new TransmissionDataWelcome;
		newPacket->mPacketType = kWelcomeCC;
		AddInFlightPacket(newPacket, outputStream);
		
		outputStream.Write(kWelcomeCC);
		//we'll assign the next possible player id to this player
		mHighestPlayerId++;
		outputStream.Write(mHighestPlayerId);
		newPacket->mHighestID = mHighestPlayerId;
		//write our player id
		outputStream.Write(mPlayerId);
		newPacket->mID = mPlayerId;
		outputStream.Write(mPlayerCount);
		//now write the player to socket map
		for (auto& iter : mPlayerToSocketMap)
		{
			outputStream.Write(iter.first);
			iter.second.Write(outputStream);
		}

		//and the player names
		for (auto& iter : mPlayerNameMap)
		{
			outputStream.Write(iter.first);
			outputStream.Write(iter.second);
		}
		std::cout << "Sending welcome" << std::endl;
		WritePendingAcks(outputStream);
		SendPacket(outputStream, inFromAddress);

		//increment the player count and add this player to maps
		mPlayerCount++;
		mPlayerToSocketMap.emplace(mHighestPlayerId, inFromAddress);
		mSocketToPlayerMap.emplace(inFromAddress, mHighestPlayerId);
		mPlayerNameMap.emplace(mHighestPlayerId, name);
	}
	else
	{
		string name;
		inInputStream.Read(name);
		//talk to the master peer, not me

		TransmissionData* newData = new TransmissionData;

		OutputMemoryBitStream outPacket;
		AddInFlightPacket(newData, outPacket);
		newData->mPacketType = kNotMasterPeerCC;
		outPacket.Write(kNotMasterPeerCC);
		mMasterPeerAddr.Write(outPacket);
		WritePendingAcks(outPacket);
		SendPacket(outPacket, inFromAddress);
	}
}

void NetworkManager::HandleIntroPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
	//master peer can safely ignore the intro packet
	if (!mIsMasterPeer)
	{
		std::cout << "handling intro" << std::endl;
		//just contains player's id and name
		int playerId;
		string name;
		inInputStream.Read(playerId);
		inInputStream.Read(name);
		UpdateHighestPlayerId(playerId);
		mPlayerCount++;

		//add the new player to our maps
		mPlayerToSocketMap.emplace(playerId, inFromAddress);
		mSocketToPlayerMap.emplace(inFromAddress, playerId);
		mPlayerNameMap.emplace(playerId, name);
	}
	else //Masterpeer does need to read in the info in order for the acknowledgement to work
	{
		int playerId;
		string name;
		inInputStream.Read(playerId); //Just read them in, doesn't need to do anything else
		inInputStream.Read(name);
	}
}

void NetworkManager::HandleStartPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
	UNREFERENCED_PARAMETER(inInputStream);
	//make sure this is the master peer, cause we don't want any funny business
	if (inFromAddress == mMasterPeerAddr)
	{
		LOG("Got the orders to go!");
		std::cout << "Handling start" << std::endl;
		//for now, assume that we're one frame off, but ideally we would RTT to adjust
		//the time to start, based on latency/jitter
		mState = NetworkManagerState::NMS_Starting;
		mTimeToStart = kStartDelay - Timing::sInstance.GetDeltaTime();
		
	}
}

void NetworkManager::ProcessPacketsPlaying(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
	uint32_t	packetType;
	inInputStream.Read(packetType);
	
	switch (packetType)
	{
	case kUpdateCC:
		HandleUpdatePacket(inInputStream, inFromAddress);
		break;
	default:
		//ignore anything else
		std::cout << "anything else" << std::endl;
		break;
	}
}

void NetworkManager::HandleUpdatePacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
	if (mSocketToPlayerMap.find(inFromAddress) != mSocketToPlayerMap.end())
	{
		int expectedId = mSocketToPlayerMap[inFromAddress];
	
		int playerId;
		//inInputStream.Read(turnNum);
		inInputStream.Read(playerId);

		if (playerId != expectedId)
		{
			std::cout << "We received turn data for a different player Id...stop trying to cheat!" << std::endl;
			return;
		}
		uint32_t size;
		inInputStream.Read(size);
		//std::cout << "Num actions read: " << size << std::endl;
		if (size > 0)
		{
			for (int i = 0; i < size; i++)
			{
				ActionData data;
				data.Read(inInputStream);
				Game::getInstance()->HandleAction(data.type, data.postion, data.seed);

			}
		}

		

	}
}

void NetworkManager::ProcessPacketsDelay(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
	//the only packet we can even consider here is an input one, since we
	//only can only enter delay after we've been playing
	uint32_t	packetType;
	inInputStream.Read(packetType);

	if (packetType == kUpdateCC)
	{
		HandleUpdatePacket(inInputStream, inFromAddress);
		//if we're lucky, maybe this was the packet we were waiting on?
		//TryAdvanceTurn();
	}
}

void NetworkManager::HandleConnectionReset(const SocketAddress& inFromAddress)
{
	//remove this player from our maps
	if (mSocketToPlayerMap.find(inFromAddress) != mSocketToPlayerMap.end())
	{
		uint32_t playerId = mSocketToPlayerMap[inFromAddress];
		mSocketToPlayerMap.erase(inFromAddress);
		mPlayerToSocketMap.erase(playerId);
		mPlayerNameMap.erase(playerId);
		//ScoreBoardManager::sInstance->RemoveEntry(playerId);

		mPlayerCount--;

		//if this was the master peer, pick the next player in the string map to be MP
		if (inFromAddress == mMasterPeerAddr)
		{
			uint32_t newMPId = mPlayerNameMap.begin()->first;
			if (newMPId == mPlayerId)
			{
				//I'm the new master peer, muahahahah
				mIsMasterPeer = true;
			}
			else
			{
				mMasterPeerAddr = mPlayerToSocketMap[newMPId];
			}
		}

		//if we were in delay, then let's see if we can continue now that this player DC'd?
		if (mState == NetworkManagerState::NMS_Delay)
		{
			//TryAdvanceTurn();
		}
	}
}

void NetworkManager::ReadIncomingPacketsIntoQueue()
{
	//should we just keep a static one?
	char packetMem[1500];
	int packetSize = sizeof(packetMem);
	InputMemoryBitStream inputStream(packetMem, packetSize * 8);
	SocketAddress fromAddress;

	//keep reading until we don't have anything to read ( or we hit a max number that we'll process per frame )
	int receivedPackedCount = 0;
	int totalReadByteCount = 0;

	while (receivedPackedCount < kMaxPacketsPerFrameCount)
	{
		int readByteCount = mSocket->ReceiveFrom(packetMem, packetSize, fromAddress);
		if (readByteCount == 0)
		{
			//nothing to read
			break;
		}
		else if (readByteCount == -WSAECONNRESET)
		{
			//port closed on other end, so DC this person immediately
			HandleConnectionReset(fromAddress);
		}
		else if (readByteCount > 0)
		{
			inputStream.ResetToCapacity(readByteCount);
			++receivedPackedCount;
			totalReadByteCount += readByteCount;

			//now, should we drop the packet?
			if (RoboMath::GetRandomFloatNonGame() >= mDropPacketChance && mState >= NetworkManagerState::NMS_Playing)
			{
				//we made it
				//shove the packet into the queue and we'll handle it as soon as we should...
				//we'll pretend it wasn't received until simulated latency from now
				float simulatedReceivedTime = Timing::sInstance.GetTimef() + mSimulatedLatency;

				//Simulating Jitter
				float simulatedJitter = RoboMath::GetRandomFloat(-0.5f,0.5f);
				simulatedReceivedTime += simulatedJitter;

				mPacketQueue.emplace(simulatedReceivedTime, inputStream, fromAddress);
			}
			else if (mState < NetworkManagerState::NMS_Playing) //Here so that it doesn't drop packets before the game has started
			{
				float simulatedReceivedTime = Timing::sInstance.GetTimef() + mSimulatedLatency;

				mPacketQueue.emplace(simulatedReceivedTime, inputStream, fromAddress);
			}
			else
			{
				//LOG("Dropped packet!", 0);
				//dropped!
			}
		}
		else
		{
			//uhoh, error? exit or just keep going?
		}
	}

	if (totalReadByteCount > 0)
	{
		mBytesReceivedPerSecond.UpdatePerSecond(static_cast<float>(totalReadByteCount));
	}
}

void NetworkManager::ProcessQueuedPackets()
{
	//look at the front packet...
	while (!mPacketQueue.empty())
	{
		ReceivedPacket& nextPacket = mPacketQueue.front();
		if (Timing::sInstance.GetTimef() > nextPacket.GetReceivedTime())
		{
			ProcessPacket(nextPacket.GetPacketBuffer(), nextPacket.GetFromAddress());
			mPacketQueue.pop();
		}
		else
		{
			break;
		}

	}

}

void NetworkManager::UpdateHighestPlayerId(uint32_t inId)
{
	mHighestPlayerId = std::max(mHighestPlayerId, inId);
}

void NetworkManager::EnterPlayingState()
{
	mState = NetworkManagerState::NMS_Playing;
	Game::getInstance()->startGame();
}

void NetworkManager::SendPacket(const OutputMemoryBitStream& inOutputStream, const SocketAddress& inToAddress)
{
	int sentByteCount = mSocket->SendTo(inOutputStream.GetBufferPtr(), inOutputStream.GetByteLength(), inToAddress);
	if (sentByteCount > 0)
	{
		mBytesSentThisFrame += sentByteCount;
	}
}

void NetworkManager::TryStartGame()
{
	if (mState < NetworkManagerState::NMS_Starting && IsMasterPeer())
	{
		LOG("Master peer starting the game!");
		std::cout << "Sending start" << std::endl;
		//let everyone know
		OutputMemoryBitStream outPacket;
		TransmissionDataStart* newData = new TransmissionDataStart;
		newData->mPacketType = kStartCC;
		AddInFlightPacket(newData, outPacket);
		
		outPacket.Write(kStartCC);
		WritePendingAcks(outPacket);
		for (auto& iter : mPlayerToSocketMap)
		{
			SendPacket(outPacket, iter.second);
		}

		mTimeToStart = kStartDelay;
		mState = NetworkManagerState::NMS_Starting;
	}
}

void NetworkManager::UpdateBytesSentLastFrame()
{
	if (mBytesSentThisFrame > 0)
	{
		mBytesSentPerSecond.UpdatePerSecond(static_cast<float>(mBytesSentThisFrame));

		mBytesSentThisFrame = 0;
	}

}


NetworkManager::ReceivedPacket::ReceivedPacket(float inReceivedTime, InputMemoryBitStream& ioInputMemoryBitStream, const SocketAddress& inFromAddress) :
	mReceivedTime(inReceivedTime),
	mFromAddress(inFromAddress),
	mPacketBuffer(ioInputMemoryBitStream)
{
}

void NetworkManager::addAction(Game::ActionTypes type, Vector2D pos, int seed)
{
	ActionData action;
	action.type = type;
	action.postion = pos;
	action.seed = seed;
	mActionVec.push_back(action);
}

void NetworkManager::ActionData::Write(OutputMemoryBitStream& inOutputStream)
{
	inOutputStream.Write(type);
	inOutputStream.Write(postion.getX());
	inOutputStream.Write(postion.getY());
	if (type == Game::ActionTypes::CreateUnitRand || type == Game::ActionTypes::CreateUnitMove)
	{
		inOutputStream.Write(seed);
	}
}

void NetworkManager::ActionData::Read(InputMemoryBitStream& inInputStream)
{
	float x, y;
	inInputStream.Read(type);
	inInputStream.Read(x);
	inInputStream.Read(y);
	postion = Vector2D(x, y);
	if (type == Game::ActionTypes::CreateUnitRand || type == Game::ActionTypes::CreateUnitMove)
	{
		inInputStream.Read(seed);
	}
}

void NetworkManager::AddInFlightPacket(TransmissionData* data, OutputMemoryBitStream& output)
{
	int sequenceNumber = mNextOutgoingSequenceNumber;
	output.Write(sequenceNumber);
	//std::cout << "Sending packet #" << sequenceNumber << " " <<std::endl;
	++mDispatchedPacketCount;

	data->sequenceID = sequenceNumber;

	mInFlightPackets.emplace(sequenceNumber, data);
	mNextOutgoingSequenceNumber++;
}

void NetworkManager::WritePendingAcks(OutputMemoryBitStream& output)
{
	uint32_t size = mPendingAcks.size();
	output.Write(size);
	//std::cout << "Num of acks " << size << std::endl;
	if (size > 0)
	{
		for (int i = 0; i < size; i++)
		{
			output.Write(mPendingAcks[i]);
			//std::cout << mPendingAcks[i] << std::endl;
		}
	}
	mPendingAcks.clear();
}

void NetworkManager::ProcessNewAcks(InputMemoryBitStream& input)
{
	TransmissionData* temp;
	uint32_t Acks;
	input.Read(Acks);
	//std::cout << "Acks: " << Acks << std::endl;
	if (Acks > 0)
	{
		for (int i = 0; i < Acks; i++)
		{
			int sequenceNum;
			input.Read(sequenceNum);
			int size = mInFlightPackets.size();
			map<int, TransmissionData*>::iterator itr = mInFlightPackets.begin();
			for (int j = 0; j < size; j++)
			{
				if (itr->first < sequenceNum)
				{
					//std::cout << itr->first << " < " << sequenceNum << std::endl;
					temp = itr->second;
					temp->handleDeliveryFailure();
					mInFlightPackets.erase(itr);
					itr = mInFlightPackets.begin();
					delete(temp);
					temp = nullptr;
				}
				else if (itr->first == sequenceNum)
				{
					temp = itr->second;
					//std::cout << sequenceNum << " Packet delivered" << std::endl;
					temp->handleDeliverySuccess();
					mInFlightPackets.erase(itr);
					itr = mInFlightPackets.begin();
					delete(temp);
					temp = nullptr;
				}
				else if (itr->first > sequenceNum)
				{
					//std::cout << sequenceNum << " No news" << std::endl;
					break;
				}
			}
		}
	}
}

void NetworkManager::ProcessSequenceNum(int sequenceNum)
{
	if (sequenceNum == mNextExpectedSequenceNumber)
	{
		//std::cout << sequenceNum << " equals " << mNextExpectedSequenceNumber << std::endl;
		mNextExpectedSequenceNumber = sequenceNum + 1;
		mPendingAcks.push_back(sequenceNum);
	}
	else if (sequenceNum > mNextExpectedSequenceNumber)
	{
		//std::cout << sequenceNum << " greater than " << mNextExpectedSequenceNumber << std::endl;
		mNextExpectedSequenceNumber = sequenceNum + 1;
		
		mPendingAcks.push_back(sequenceNum);
		
	}
	else
	{
		//std::cout << sequenceNum << " less than " << mNextExpectedSequenceNumber << std::endl;
		return;
	}

}



void NetworkManager::TransmissionDataUpdate::handleDeliveryFailure()
{
	if (mData.size() > 0)
	{
		std::cout << sequenceID << " Action packet dropped, resending" << std::endl;
		NetworkManager* pNetManager = Game::getInstance()->getNetworkManager();
		for (int i = 0; i < mData.size(); i++)
		{
			pNetManager->mActionVec.push_back(mData[i]);
		}
		mData.clear();
	}
}

void NetworkManager::TransmissionDataHello::handleDeliveryFailure()
{
	std::cout << sequenceID << " hello packet dropped" << std::endl;
}

void NetworkManager::TransmissionDataIntro::handleDeliveryFailure()
{
	std::cout << sequenceID << " intro packet dropped, resending" << std::endl;
}

void NetworkManager::TransmissionDataWelcome::handleDeliveryFailure()
{
	std::cout << sequenceID << " welcome packet dropped, resending" << std::endl;
}
void NetworkManager::TransmissionDataStart::handleDeliveryFailure()
{
	std::cout << sequenceID << " start packet dropped, resending" << std::endl;
	NetworkManager* pNetManager = Game::getInstance()->getNetworkManager();
	pNetManager->TryStartGame();
}