#include "Newtorker.h"

//Constructor
Networker::Networker()
{
	mTCPSocket = nullptr;
}

Networker::~Networker()
{
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

//TO DO: FINISH THIS
void Networker::GetNewGameObjectState(GameObject* gameObject)
{
	char buffer[1024];
	int32_t byteRecieve = mTCPSocket->Receive(buffer, 1024);
	if (byteRecieve > 0)
	{
		InputMemoryBitStream IMBStream = InputMemoryBitStream(buffer, 1024);
		switch (gameObject->getGameObjectID())
		{
		case GameObjectType::ROCK:

			break;

		case GameObjectType::WALL:

			break;

		default:
			break;
		}

	}
	else if (byteRecieve < 0)
	{
		LOG("%s", "User has disconnected");
	}
}

//TO DO: FINISH THIS
void Networker::SendNewGameObjectState(GameObject* gameObject)
{
	OutputMemoryBitStream OMBStream;
	OMBStream.Write(gameObject->getGameObjectID());
	//Get data from serialization and write it OMBStream.Write(DATA HERE);

	mTCPSocket->Send(OMBStream.GetBufferPtr(), OMBStream.GetBitLength());
}