#include "Newtorker.h"

//Constructor
Networker::Networker()
{
	mTCPSocket = nullptr;
}

Networker::~Networker()
{
	SocketUtil::CleanUp();
}

void Networker::InitServer()
{
	SocketUtil::StaticInit();

	//Create Socket
	mTCPSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (mTCPSocket == nullptr)
	{
		SocketUtil::ReportError("Creating Listenting Socket");
		ExitProcess(1);
	}
	LOG("%s", "Listening Socket Succesfully Created!");

	//Create and Bind Address
	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	if (listenAddress == nullptr)
	{
		SocketUtil::ReportError("Creating Listening Address");
		ExitProcess(1);
	}

	if (mTCPSocket->Bind(*listenAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding listening socket");
		ExitProcess(1);
	}
	LOG("%s", "Listening Socket Succesfully Binded!");

	if (mTCPSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on socket");
		ExitProcess(1);
	}
	LOG("%s", "Listening Socket Listening");

	//Accept Connection
	LOG("%s", "Waiting for connection...");
	mTCPSocket->SetNonBlockingMode(false);
	
	SocketAddress incomingAddress;
	TCPSocketPtr connSocket = mTCPSocket->Accept(incomingAddress);

	mTCPSocket->~TCPSocket();
	mTCPSocket = connSocket;
	mTCPSocket->SetNonBlockingMode(false);
	LOG("Accpted connection from address: %s", incomingAddress.ToString().c_str());
}

void Networker::Connect(string IPAddress)
{
	SocketUtil::StaticInit();

	//Create Socket
	mTCPSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (mTCPSocket == nullptr)
	{
		SocketUtil::ReportError("Creating Client Socket");
		ExitProcess(1);
		return;
	}

	string address = "127.0.0.2:8080";
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError("Creating Client Address");
		ExitProcess(1);
		return;
	}

	if (mTCPSocket->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding Client Socket");
		ExitProcess(1);
	}
	LOG("%s", "Client Socket Succesfully Binded!");

	SocketAddressPtr srvAddress = SocketAddressFactory::CreateIPv4FromString(IPAddress);
	if (srvAddress == nullptr)
	{
		SocketUtil::ReportError("Creating Server Address");
		ExitProcess(1);
	}

	if (mTCPSocket->Connect(*srvAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting To Server");
		ExitProcess(1);
	}
	LOG("%s", "Succesfully Connect to the Server!");

	mTCPSocket->SetNonBlockingMode(true);

}


void Networker::GetNewGameObjectState(map<int, GameObject*> gameObjectMap)
{
	char buffer[1024];
	int32_t byteRecieve = mTCPSocket->Receive(buffer, 1024);
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


void Networker::SendNewGameObjectState(map<int, GameObject*> gameObjectMap, int ID)
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

	mTCPSocket->Send(OMBStream.GetBufferPtr(), OMBStream.GetBitLength());
}