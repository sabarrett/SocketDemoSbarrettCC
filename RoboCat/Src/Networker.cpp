#include "Networker.h"
#include <iostream>

//Constructor
Networker::Networker()
{
	mbIsInitted = false;
}

Networker::~Networker()
{
	cleanup();
}

void Networker::init(GraphicsLibrary* graphicsLibrary, std::string rockSpriteIdentifier, std::string playerSpriteIdentifier, float playerMoveSpeed, Colour wallColour/*, float arrivalTime*/)
{
	if (mbIsInitted)
	{
		cleanup();
	}

	std::srand(time(NULL));

	mpUDPSocket = new UDPSocketPtr();
	mpSocketAddressPtr = new SocketAddressPtr();
	mGameObjectsVec = std::vector<std::pair<int, GameObject*>>();
	pDeliveryNotificationManager = new DeliveryNotificationManager(true, true, this);
	mOutputBitStreamQueue = std::priority_queue<std::pair<float, OutputMemoryBitStream*>, std::vector<std::pair<float, OutputMemoryBitStream*>>, myComp>();

	//Data for GameObject replication
	mpGraphicsLibrary = graphicsLibrary;
	mRockSpriteIdentifier = rockSpriteIdentifier;
	mPlayerSpriteIdentifier = playerSpriteIdentifier;
	mPlayerMoveSpeed = playerMoveSpeed;
	mWallColour = wallColour;

	mbIsInitted = true;
}

void Networker::cleanup()
{
	//Cleanup map
	std::vector<std::pair<int, GameObject*>>::iterator it;
	for (it = mGameObjectsVec.begin(); it != mGameObjectsVec.end(); ++it)
	{
		delete it->second;
		it->second = nullptr;
	}
	mGameObjectsVec.clear();

	//Cleanup delivery notification manager
	delete pDeliveryNotificationManager;
	pDeliveryNotificationManager = nullptr;

	(*mpUDPSocket)->CleanupSocket();
	delete mpUDPSocket;
	mpUDPSocket = nullptr;

	delete mpSocketAddressPtr;
	mpSocketAddressPtr = nullptr;

	SocketUtil::CleanUp();

	mbIsInitted = false;
}

bool Networker::initServerUDP(std::string clientIpAddress, std::string servPort, std::string clientPort)
{
	SocketUtil::StaticInit();

	//Create Socket
	UDPSocketPtr sock = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	sock->SetNonBlockingMode(true);

	if (sock == nullptr)
	{
		SocketUtil::ReportError("Creating Client Socket");
		ExitProcess(1);
		return false;
	}

	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString((clientIpAddress + ":" + clientPort).c_str());
	SocketAddressPtr sockAddress = SocketAddressFactory::CreateIPv4FromString(("0.0.0.0:" + servPort).c_str());
	if (sockAddress == nullptr)
	{
		SocketUtil::ReportError("Creating Server Address");
		ExitProcess(1);
	}

	if (sock->Bind(*sockAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding Server Socket");
		ExitProcess(1);
	}
	LOG("%s", "Server Socket Succesfully Binded!");

	*mpSocketAddressPtr = clientAddress;
	*mpUDPSocket = sock;

	if (*mpUDPSocket != nullptr)
		return true;
	return false;
}

bool Networker::connectUDP(std::string serverIpAddress, std::string servPort, std::string clientPort)
{
	SocketUtil::StaticInit();

	//Create Socket
	UDPSocketPtr sock = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	sock->SetNonBlockingMode(true);

	if (sock == nullptr)
	{
		SocketUtil::ReportError("Creating Client Socket");
		ExitProcess(1);
		return false;
	}

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString((serverIpAddress + ":" + servPort).c_str());
	SocketAddressPtr sockAddress = SocketAddressFactory::CreateIPv4FromString(("0.0.0.0:" + clientPort).c_str());
	if (sockAddress == nullptr)
	{
		SocketUtil::ReportError("Creating Server Address");
		ExitProcess(1);
	}

	if (sock->Bind(*sockAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding Client Socket");
		ExitProcess(1);
	}
	LOG("%s", "Server Socket Succesfully Binded!");

	*mpUDPSocket = sock;
	*mpSocketAddressPtr = servAddress;

	if (*mpUDPSocket != nullptr)
		return true;
	return false;
}

PacketType Networker::receiveGameObjectStateUDP()
{
	char buffer[1024];
	int32_t byteRecieve = (*mpUDPSocket)->ReceiveFrom(buffer, 1024, (**mpSocketAddressPtr));

	if (byteRecieve > 0)
	{
		InputMemoryBitStream IMBStream = InputMemoryBitStream(buffer, 1024);

		//Read sequence number
		if (pDeliveryNotificationManager->ReadAndProcessState(IMBStream))
		{
			//Start reading
			PacketType packetHeader;
			IMBStream.Read(packetHeader);
			int networkID;
			IMBStream.Read(networkID);

			//If ID received is < 0, we are doing connection stuff
			if (networkID < 0)
			{
				return packetHeader;
			}

			GameObjectType receiveType;
			IMBStream.Read(receiveType);

			//Logic depends on packer header type
			switch (packetHeader)
			{
			case PacketType::PACKET_HELLO:
				break;

			case PacketType::PACKET_CREATE:
			{
				float posX;
				float posY;

				IMBStream.Read(posX);
				IMBStream.Read(posY);

				switch (receiveType)
				{
				case GameObjectType::ROCK:
				{
					Rock* newRock = new Rock(networkID, mpGraphicsLibrary, pair<float, float>(posX, posY), mRockSpriteIdentifier);
					mGameObjectsVec.push_back(pair<int, GameObject*>(networkID, newRock));
					newRock = nullptr;

					break;
				}

				case GameObjectType::PLAYER:
				{
					PlayerController* newPlayerController = new PlayerController(networkID, mpGraphicsLibrary, pair<float, float>(posX, posY), mPlayerMoveSpeed, mPlayerSpriteIdentifier);
					mGameObjectsVec.push_back(pair<int, GameObject*>(networkID, newPlayerController));
					newPlayerController = nullptr;

					break;

				}

				case GameObjectType::WALL:
				{
					float sizeX;
					float sizeY;
					float thickness;

					IMBStream.Read(sizeX);
					IMBStream.Read(sizeY);
					IMBStream.Read(thickness);

					Wall* newWall = new Wall(networkID, mpGraphicsLibrary, pair<float, float>(posX, posY), sizeX, sizeY, mWallColour, thickness);
					mGameObjectsVec.push_back(pair<int, GameObject*>(networkID, newWall));
					newWall = nullptr;

					break;
				}
				}

				break;
			}

			case PacketType::PACKET_UPDATE:

				if (mGameObjectsVec[networkID].second != nullptr)
				{
					float x;
					float y;

					switch (receiveType)
					{
					case GameObjectType::ROCK:
					case GameObjectType::PLAYER:

						IMBStream.Read(x);
						IMBStream.Read(y);

						//Checking to absolutly make sure values are getting recieved
						std::cout << "Recieved Position X: " << x << std::endl;
						std::cout << "Recieved Position Y: " << y << std::endl;

						mGameObjectsVec[networkID].second->setPos(std::make_pair(x, y));
						break;

					case GameObjectType::WALL:
					{
						float sizeX;
						float sizeY;
						float thiccness;

						Wall* wall = (Wall*)mGameObjectsVec[networkID].second;
						IMBStream.Read(x);
						IMBStream.Read(y);

						wall->setPos(std::make_pair(x, y));

						IMBStream.Read(sizeX);
						IMBStream.Read(sizeY);
						IMBStream.Read(thiccness);

						wall->setWallSizeX(sizeX);
						wall->setWallSizeY(sizeY);
						wall->setWallThickness(thiccness);

						break;
					}
					}
				}
				else
				{
					//Report error
					std::cout << "ERROR: CANNOT UPDATE GAMEOBJECT ID " << networkID << " BECAUSE IT IS NOT IN THE NETWORK MANAGER MAP.\n";
				}

				break;

			case PacketType::PACKET_DELETE:
			{
				//Delete element in map
				if (mGameObjectsVec.size() > 0)
				{
					std::vector<std::pair<int, GameObject*>>::iterator it;
					for (it = mGameObjectsVec.begin(); it != mGameObjectsVec.end(); ++it)
					{
						//DO NOT DELETE A PLAYER
						if (it->first == networkID && it->second->getGameObjectType() != GameObjectType::PLAYER)
						{
							mGameObjectsVec.erase(it);

							break;
						}
					}
				}

				break;
			}

			default:
				return PacketType::PACKET_INVALID;
			}

			return packetHeader;
		}
	}
	else if (byteRecieve == -10053 /*|| byteRecieve == -10054*/)
	{
		LOG("%s", "Disconnected From Server");
		exit(0);
	}
	return PacketType::PACKET_INVALID;
}

void Networker::sendGameObjectStateUDP(int ID, PacketType packetHeader)
{
	OutputMemoryBitStream* OMBStream = new OutputMemoryBitStream();

	//Write state sent (packet sequence number and acks)
	InFlightPacket* pInFlightPacket = pDeliveryNotificationManager->WriteState(*OMBStream);

	//Write packet header
	OMBStream->Write(packetHeader);

	//If ID received is < 0, we are doing connection stuff
	if (ID < 0)
	{
		//Write ID
		OMBStream->Write(ID);

		//Send packet and return from the function
		(*mpUDPSocket)->SendTo(OMBStream->GetBufferPtr(), OMBStream->GetBitLength(), (**mpSocketAddressPtr));
		
		delete OMBStream;
		OMBStream = nullptr;
		return;
	}

	OMBStream->Write(mGameObjectsVec[ID].second->getNetworkID());
	OMBStream->Write(mGameObjectsVec[ID].second->getGameObjectType());

	//Logic depends on packer header type
	switch (packetHeader)
	{
	case PacketType::PACKET_CREATE:
	case PacketType::PACKET_UPDATE:

		switch (mGameObjectsVec[ID].second->getGameObjectType())
		{
		case GameObjectType::ROCK:
		case GameObjectType::PLAYER:

			//For testing to absolutly make sure values are getting written
			std::cout << "Position X: " << mGameObjectsVec[ID].second->getPosition().first << std::endl;
			std::cout << "Position Y: " << mGameObjectsVec[ID].second->getPosition().second << std::endl;

			OMBStream->Write(mGameObjectsVec[ID].second->getPosition().first);
			OMBStream->Write(mGameObjectsVec[ID].second->getPosition().second);
			pInFlightPacket->SetTransmissionData(0, std::shared_ptr<GameObject>(mGameObjectsVec[ID].second));
			//pInFlightPacket = nullptr;
			break;

		case GameObjectType::WALL:
			Wall* wall = (Wall*)mGameObjectsVec[ID].second;
			OMBStream->Write(wall->getPosition().first);
			OMBStream->Write(wall->getPosition().second);
			OMBStream->Write(wall->getWallSizeX());
			OMBStream->Write(wall->getWallSizeY());
			OMBStream->Write(wall->getWallThickness());
			pInFlightPacket->SetTransmissionData(0, std::shared_ptr<GameObject>(wall));
			//pInFlightPacket = nullptr;
			break;
		}

		break;

	case PacketType::PACKET_DELETE:
	{
		//Delete it on the sender's end
		if (mGameObjectsVec.size() > 0)
		{
			std::vector<std::pair<int, GameObject*>>::iterator it;
			for (it = mGameObjectsVec.begin(); it != mGameObjectsVec.end(); ++it)
			{
				//DO NOT DELETE A PLAYER
				if (it->first == ID && it->second->getGameObjectType() != GameObjectType::PLAYER)
				{
					mGameObjectsVec.erase(it);
					pInFlightPacket->SetTransmissionData(0, std::shared_ptr<GameObject>(it->second));
					//pInFlightPacket = nullptr;

					break;
				}
			}
		}
	}

	break;

	default:
		return;
	}

	//Make sure to send the first two gameobjects so both players can connect and be displayed on screen
	if (ID > 1)
	{
		//Add packet data to queue in randomised order --> done through timeDispatched +- random and the priority_queue's sorting
		float timeDispatched = Timing::sInstance.GetTimef() + (-100 + rand() & (100 - -100 + 1));
		mOutputBitStreamQueue.push(std::make_pair(timeDispatched, OMBStream));
		OMBStream = nullptr;

		//If the queue has more than 10 elements is it
		if (mOutputBitStreamQueue.size() >= 10)
		{
			//Iterate though the queue
			//for (size_t i = mOutputBitStreamQueue.size() - 1; i >= 0 ; i--)
			for (size_t i = 0; i < mOutputBitStreamQueue.size(); i++)
			{
				//Drop some packets
				if (i % 3 != 0)
				{
					//Send packet
					(*mpUDPSocket)->SendTo(mOutputBitStreamQueue.top().second->GetBufferPtr(), mOutputBitStreamQueue.top().second->GetBitLength(), (**mpSocketAddressPtr));
					delete mOutputBitStreamQueue.top().second;
					mOutputBitStreamQueue.pop();
				}
			}
		}
	}
	else
	{
		//Send packet
		(*mpUDPSocket)->SendTo(OMBStream->GetBufferPtr(), OMBStream->GetBitLength(), (**mpSocketAddressPtr));

		delete OMBStream;
		OMBStream = nullptr;
	}
}

void Networker::checkTimedOutPackets()
{
	pDeliveryNotificationManager->ProcessTimedOutPackets();
}

void Networker::addGameObject(GameObject* objectToAdd, int networkID)
{
	mGameObjectsVec.push_back(pair<int, GameObject*>(networkID, objectToAdd));
}

void Networker::updateGameObjects()
{
	std::vector<std::pair<int, GameObject*>>::iterator it;
	for (it = mGameObjectsVec.begin(); it != mGameObjectsVec.end(); ++it)
	{
		it->second->update();
	}
}

void Networker::renderGameObjects()
{
	std::vector<std::pair<int, GameObject*>>::iterator it;
	for (it = mGameObjectsVec.begin(); it != mGameObjectsVec.end(); ++it)
	{
		it->second->draw();
	}
}