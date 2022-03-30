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

void Networker::init(GraphicsLibrary* graphicsLibrary, std::string rockSpriteIdentifier, std::string playerSpriteIdentifier, float playerMoveSpeed, Colour wallColour)
{
	mpTCPSocket = new TCPSocketPtr;
	mGameObjectMap = map<int, GameObject*>();

	//Data for GameObject replication
	mpGraphicsLibrary = graphicsLibrary;
	mRockSpriteIdentifier = rockSpriteIdentifier;
	mPlayerSpriteIdentifier = playerSpriteIdentifier;
	mPlayerMoveSpeed = playerMoveSpeed;
	mWallColour = wallColour;
}

void Networker::cleanup()
{
	//delete mInstance;
	//mInstance = nullptr;

	//Cleanup map
	map<int, GameObject*>::iterator it;
	for (it = mGameObjectMap.begin(); it != mGameObjectMap.end(); ++it)
	{
		delete it->second;
		it->second = nullptr;
	}
	mGameObjectMap.clear();

	(*mpTCPSocket)->CleanupSocket();
	SocketUtil::CleanUp();
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
	//*mpTCPSocket = sock;

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


void Networker::getNewGameObjectState()
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
				mGameObjectMap.insert(pair<int, GameObject*>(networkID, newRock));
				newRock = nullptr;

				break;
			}

			case GameObjectType::PLAYER:
			{
				PlayerController* newPlayerController = new PlayerController(networkID, mpGraphicsLibrary, pair<float, float>(posX, posY), mPlayerMoveSpeed, mPlayerSpriteIdentifier);
				mGameObjectMap.insert(pair<int, GameObject*>(networkID, newPlayerController));
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
				mGameObjectMap.insert(pair<int, GameObject*>(networkID, newWall));
				newWall = nullptr;

				break;
			}

				break;
			}

			break;
		}

		case PacketType::PACKET_UPDATE:

			if (mGameObjectMap[networkID] != nullptr)
			{
				float x;
				float y;

				switch (receiveType)
				{
				case GameObjectType::ROCK:
				case GameObjectType::PLAYER:

					IMBStream.Read(x);
					IMBStream.Read(y);
					mGameObjectMap[networkID]->setPos(std::make_pair(x, y));
					break;

				case GameObjectType::WALL:
				{
					float sizeX;
					float sizeY;
					float thiccness;

					Wall* wall = (Wall*)mGameObjectMap[networkID];
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
			map<int, GameObject*>::iterator it;
			for (it = mGameObjectMap.begin(); it != mGameObjectMap.end(); ++it)
			{
				if (it->first == networkID)
				{
					delete it->second;
					it->second = nullptr;
					mGameObjectMap.erase(it->first);
				}
			}

			break;
		}

		default:
			return;
		}
	}
	else if (byteRecieve == -10053)
	{
		LOG("%s", "Disconnected From Server");
		exit(0);
	}
}

void Networker::sendNewGameObjectState(int ID, PacketType packetHeader)
{
	OutputMemoryBitStream OMBStream;
	OMBStream.Write(packetHeader);
	OMBStream.Write(mGameObjectMap[ID]->getNetworkID());
	OMBStream.Write(mGameObjectMap[ID]->getGameObjectType());

	//Logic depends on packer header type
	switch (packetHeader)
	{
	case PacketType::PACKET_CREATE:
	case PacketType::PACKET_UPDATE:

		switch (mGameObjectMap[ID]->getGameObjectType())
		{
		case GameObjectType::ROCK:
		case GameObjectType::PLAYER:
			OMBStream.Write(mGameObjectMap[ID]->getPosition().first);
			OMBStream.Write(mGameObjectMap[ID]->getPosition().second);
			break;

		case GameObjectType::WALL:
			Wall* wall = (Wall*)mGameObjectMap[ID];
			OMBStream.Write(wall->getPosition().first);
			OMBStream.Write(wall->getPosition().second);
			OMBStream.Write(wall->getWallSizeX());
			OMBStream.Write(wall->getWallSizeY());
			OMBStream.Write(wall->getWallThickness());
			break;
		}

		break;

	case PacketType::PACKET_DELETE:

		//Send nothing else

		break;

	default:
		return;
	}

	(*mpTCPSocket)->Send(OMBStream.GetBufferPtr(), OMBStream.GetBitLength());
}

void Networker::addGameObjectToMap(GameObject* objectToAdd, int networkID)
{
	mGameObjectMap.insert(pair<int, GameObject*>(networkID, objectToAdd));
}

void Networker::updateMapObjects()
{
	map<int, GameObject*>::iterator it;
	for (it = mGameObjectMap.begin(); it != mGameObjectMap.end(); ++it)
	{
		it->second->update();
	}
}

void Networker::renderMapObjects()
{
	map<int, GameObject*>::iterator it;
	for (it = mGameObjectMap.begin(); it != mGameObjectMap.end(); ++it)
	{
		it->second->draw();
	}
}