#include "Networker.h"
#include <iostream>

//Constructor
Networker::Networker()
{
	init();
}

Networker::~Networker()
{
	cleanup();
	SocketUtil::CleanUp();
}

void Networker::init()
{
	mpTCPSocket = new TCPSocketPtr;
}

void Networker::cleanup()
{
	delete mInstance;
	mInstance = nullptr;

	(*mpTCPSocket)->CleanupSocket();
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
	
	SocketAddress incomingAddress;
	TCPSocketPtr connSocket = sock->Accept(incomingAddress);

	while (connSocket == nullptr)
		connSocket = sock->Accept(incomingAddress);

	mpTCPSocket = &connSocket;
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

	mpTCPSocket = &sock;

	if (mpTCPSocket != nullptr)
		return true;
	return false;
}


void Networker::getNewGameObjectState(map<int, GameObject*> gameObjectMap)
{
	char buffer[1024];
	int32_t byteRecieve = (*mpTCPSocket)->Receive(buffer, 1024);
	if (byteRecieve > 0)
	{
		InputMemoryBitStream IMBStream = InputMemoryBitStream(buffer, 1024);
		int networkID;
		IMBStream.Read(networkID);

		if (gameObjectMap[networkID] != nullptr)
		{
			GameObjectType recieveType;
			IMBStream.Read(recieveType);

			float x;
			float y;
			float sizeX;
			float sizeY;
			switch (recieveType)
			{
			case GameObjectType::ROCK:
			case GameObjectType::PLAYER:

				IMBStream.Read(x);
				IMBStream.Read(y);
				gameObjectMap[networkID]->setPos(std::make_pair(x, y));
				break;

			case GameObjectType::WALL:
				Wall* wall = (Wall*)gameObjectMap[networkID];
				IMBStream.Read(x);
				IMBStream.Read(y);
				
				wall->setPos(std::make_pair(x, y));

				IMBStream.Read(sizeX);
				IMBStream.Read(sizeY);

				wall->setWallSizeX(sizeX);
				wall->setWallSizeY(sizeY);

				break;
			}
		}
		else
		{
			//create new object
		}
	}
}


void Networker::sendNewGameObjectState(map<int, GameObject*> gameObjectMap, int ID)
{
	OutputMemoryBitStream OMBStream;
	OMBStream.Write(gameObjectMap[ID]->getNetworkID());
	OMBStream.Write(gameObjectMap[ID]->getGameObjectID());
	
	switch (gameObjectMap[ID]->getGameObjectID())
	{
	case GameObjectType::ROCK:
	case GameObjectType::PLAYER:
		OMBStream.Write(gameObjectMap[ID]->getPosition().first);
		OMBStream.Write(gameObjectMap[ID]->getPosition().second);
		break;

	case GameObjectType::WALL:
		Wall* wall = (Wall*)gameObjectMap[ID];
		OMBStream.Write(wall->getPosition().first);
		OMBStream.Write(wall->getPosition().second);
		OMBStream.Write(wall->getWallSizeX());
		OMBStream.Write(wall->getWallSizeY());
		break;
	}

	(*mpTCPSocket)->Send(OMBStream.GetBufferPtr(), OMBStream.GetBitLength());
}

void Networker::CleanupMap()
{
	map<int, GameObject*>::iterator it;
	for (it = mGameObjectMap.begin(); it != mGameObjectMap.end(); ++it)
	{
		delete it->second;
		it->second = nullptr;
		mGameObjectMap.erase(it->first);
	}
	mGameObjectMap.clear();
}

void Networker::AddGameObjectToMap(GameObject* objectToAdd)
{
	mGameObjectMap.insert(pair<int, GameObject*>(mGameObjectID, objectToAdd));
	++mGameObjectID;
}

void Networker::UpdateMapObjects()
{
	map<int, GameObject*>::iterator it;
	for (it = mGameObjectMap.begin(); it != mGameObjectMap.end(); ++it)
	{
		it->second->update();
	}
}

void Networker::RenderMapObjects()
{
	map<int, GameObject*>::iterator it;
	for (it = mGameObjectMap.begin(); it != mGameObjectMap.end(); ++it)
	{
		it->second->draw();
	}
}