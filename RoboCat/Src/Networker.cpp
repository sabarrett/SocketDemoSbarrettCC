#include "Networker.h"
#include <iostream>

//Constructor
Networker::Networker()
{
}

Networker::~Networker()
{
	cleanup();
}

void Networker::init(GraphicsLibrary* graphicsLibrary, std::string rockSpriteIdentifier, std::string playerSpriteIdentifier, float playerMoveSpeed, Colour wallColour, float arrivalTime)
{
	if (mIsInit)
	{
		cleanup();
	}

	std::srand(time(NULL));
	mArrivalTime = arrivalTime;

	mpTCPSocket = new TCPSocketPtr;
	mGameObjectsVec = std::vector<std::pair<int, GameObject*>>();
	mPacketQueue = std::queue<std::pair<int, float>>();

	//Data for GameObject replication
	mpGraphicsLibrary = graphicsLibrary;
	mRockSpriteIdentifier = rockSpriteIdentifier;
	mPlayerSpriteIdentifier = playerSpriteIdentifier;
	mPlayerMoveSpeed = playerMoveSpeed;
	mWallColour = wallColour;

	mIsInit = true;
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

	(*mpTCPSocket)->CleanupSocket();
	delete mpTCPSocket;
	mpTCPSocket = nullptr;

	SocketUtil::CleanUp();

	mIsInit = false;
}

bool Networker::initServer(std::string port)
{
	SocketUtil::StaticInit();

	//Create Socket
	TCPSocketPtr sock = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (sock == nullptr)
	{
		SocketUtil::ReportError("Creating Listenting Socket");
		ExitProcess(1);
	}
	std::cout << "Listening Socket Succesfully Created!\n";

	//Create and Bind Address
	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:" + port);
	if (listenAddress == nullptr)
	{
		SocketUtil::ReportError("Creating Listening Address");
		ExitProcess(1);
	}

	if (sock->Bind(*listenAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding listening socket");
		ExitProcess(1);
	}
	std::cout << "Listening Socket Succesfully Binded!\n";

	if (sock->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on socket");
		ExitProcess(1);
	}
	std::cout << "Listening Socket Listening\n";

	//Accept Connection
	std::cout << "Waiting for connection...\n";

	sock->SetNonBlockingMode(false);
	SocketAddress incomingAddress;
	TCPSocketPtr connSocket = sock->Accept(incomingAddress);

	while (connSocket == nullptr)
		connSocket = sock->Accept(incomingAddress);


	*mpTCPSocket = connSocket;

	std::cout << "Accepted connection from address: " << incomingAddress.ToString() << std::endl;

	if (mpTCPSocket != nullptr)
		return true;
	return false;
}

bool Networker::connect(std::string serverIpAddress, std::string port)
{
	SocketUtil::StaticInit();

	//Create Socket
	TCPSocketPtr sock = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

	if (sock == nullptr)
	{
		SocketUtil::ReportError("Creating Client Socket");
		ExitProcess(1);
		return false;
	}

	string address = "0.0.0.0:0";
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError("Creating Client Address");
		ExitProcess(1);
		return false;
	}

	if (sock->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding Client Socket");
		ExitProcess(1);
	}
	LOG("%s", "Client Socket Succesfully Binded!");

	SocketAddressPtr srvAddress = SocketAddressFactory::CreateIPv4FromString(serverIpAddress + ":" + port);
	if (srvAddress == nullptr)
	{
		SocketUtil::ReportError("Creating Server Address");
		ExitProcess(1);
	}

	if (sock->Connect(*srvAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting To Server");
		ExitProcess(1);
	}
	LOG("%s", "Succesfully Connect to the Server!");

	*mpTCPSocket = sock;

	if (mpTCPSocket != nullptr)
		return true;
	return false;
}


PacketType Networker::receiveGameObjectState()
{
	char buffer[1024];
	int32_t byteRecieve = (*mpTCPSocket)->Receive(buffer, 1024);
	if (byteRecieve > 0)
	{
		InputMemoryBitStream IMBStream = InputMemoryBitStream(buffer, 1024);

		//Start reading
		PacketType packetHeader;
		IMBStream.Read(packetHeader);
		int networkID;
		IMBStream.Read(networkID);
		GameObjectType receiveType;
		IMBStream.Read(receiveType);

		//Logic depends on packer header type
		switch (packetHeader)
		{
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
	else if (byteRecieve == -10053 || byteRecieve == -10054)
	{
		LOG("%s", "Disconnected From Server");
		exit(0);
	}
	return PacketType::PACKET_INVALID;
}

void Networker::sendGameObjectState(int ID, PacketType packetHeader)
{
	int time = mArrivalTime + (-100 + rand() & (100 - -100 + 1)); // MIN + rand() % (MAX - MIN + 1), calculates a random number int between a neg and pos value

	OutputMemoryBitStream OMBStream;
	OMBStream.Write(packetHeader);
	OMBStream.Write(mGameObjectsVec[ID].second->getNetworkID());
	OMBStream.Write(mGameObjectsVec[ID].second->getGameObjectType());

	//Logic depends on packer header type
	switch (packetHeader)
	{
	case PacketType::PACKET_CREATE:
	case PacketType::PACKET_UPDATE:

		switch (mGameObjectsVec[ID].second->getGameObjectType())
		{
		case GameObjectType::ROCK:
		case GameObjectType::PLAYER:
			OMBStream.Write(mGameObjectsVec[ID].second->getPosition().first);
			OMBStream.Write(mGameObjectsVec[ID].second->getPosition().second);
			break;

		case GameObjectType::WALL:
			Wall* wall = (Wall*)mGameObjectsVec[ID].second;
			OMBStream.Write(wall->getPosition().first);
			OMBStream.Write(wall->getPosition().second);
			OMBStream.Write(wall->getWallSizeX());
			OMBStream.Write(wall->getWallSizeY());
			OMBStream.Write(wall->getWallThickness());
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

					break;
				}
			}
		}
	}

	break;

	default:
		return;
	}

	(*mpTCPSocket)->Send(OMBStream.GetBufferPtr(), OMBStream.GetBitLength());
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


void Networker::SortPacketQueue()
{
	for (int i = 1; i <= mPacketQueue.size(); ++i)
	{
		int min = FindMinIndex(mPacketQueue.size() - i);
		InsertMinIndexToEnd(min);
	}
}


int Networker::FindMinIndex(int sortedIndex)
{
	int minIndex = -1;
	int minVal = INT_MAX;
	for (int i = 0; i < mPacketQueue.size(); i++)
	{
		std::pair<int, float> currentPacketTime = mPacketQueue.front();
		mPacketQueue.pop();

		if (currentPacketTime.first <= minVal && i <= sortedIndex)
		{
			minIndex = i;
			minVal = currentPacketTime.first;
		}
		mPacketQueue.push(currentPacketTime);
	}

	return minIndex; 
}

void Networker::InsertMinIndexToEnd(int minIndex)
{
	if (minIndex == -1)
	{
		return;
	}

	std::pair<int, float> minVal;
	for (int i = 0; i < mPacketQueue.size(); i++)
	{
		pair<int, float> currentPacket = mPacketQueue.front();
		mPacketQueue.pop();
		if (i != minIndex)
		{
			mPacketQueue.push(currentPacket);
		}
		else
		{
			minVal = currentPacket;
		}
	}
	mPacketQueue.push(minVal);
}