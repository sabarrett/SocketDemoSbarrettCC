#include "NetworkManager.h"

NetworkManager::NetworkManager()
{
	mpGame = nullptr;
	mpSocket = nullptr;
}

NetworkManager::~NetworkManager()
{
}

bool NetworkManager::initServer(std::string serverPort)
{
	SocketUtil::StaticInit();

	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}

	listenSocket->SetNonBlockingMode(true);

	std::cout << "Listening socket created" << endl;

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:" + serverPort);
	if (listenAddress == nullptr)
	{
		SocketUtil::ReportError("Creating listen address");
		ExitProcess(1);
	}

	if (listenSocket->Bind(*listenAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding listening socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound listening socket");

	// Listen() - Listen on socket -> Non-blocking; tells OS we care about incoming connections on this socket
	if (listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on listening socket");
		ExitProcess(1);
	}

	LOG("%s", "Listening on socket");

	// Accept() - Accept on socket -> Blocking; Waits for incoming connection and completes TCP handshake

	LOG("%s", "Waiting to accept connections...");
	SocketAddress incomingAddress;
	TCPSocketPtr connSocket = listenSocket->Accept(incomingAddress);
	while (connSocket == nullptr)
	{
		connSocket = listenSocket->Accept(incomingAddress);
		// SocketUtil::ReportError("Accepting connection");
		// ExitProcess(1);
	}

	LOG("Accepted connection from %s", incomingAddress.ToString().c_str());

	*mpSocket = connSocket;

	if (!mpSocket)
	{
		return false;
	}

	else
	{
		Game::initInstance();
		mpGame = Game::getInstance();
		mpGame->init();
		return true;
	}
}

bool NetworkManager::connect(std::string clientIP, std::string clientPort)
{
	SocketUtil::StaticInit();

	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
		return false;
	}

	LOG("%s", "Client socket created");

	std::string address = "0.0.0.0:0";
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError("Creating client address");
		ExitProcess(1);
		return false;
	}

	if (clientSocket->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding client socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound client socket");

	// Connect() -> Connect socket to remote host

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString(clientIP + ":" + clientPort);
	if (servAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	if (clientSocket->Connect(*servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}

	*mpSocket = clientSocket;

	if (!mpSocket)
	{
		return false;
	}
	else
	{
		Game::initInstance();
		mpGame = Game::getInstance();
		mpGame->init();
		return true;
	}
}

void NetworkManager::createObject(Game* obj, int objID)
{
	mvGameObjects.push_back(pair<Game*, int>(obj, objID));
}

void NetworkManager::updateObject()
{
	std::vector<std::pair<Game*, int>>::iterator iter;
	for (iter = mvGameObjects.begin(); iter != mvGameObjects.end(); iter++)
	{
		iter->first->update();
	}
}

void NetworkManager::renderObject()
{
	std::vector<std::pair<Game*, int>>::iterator iter;
	for (iter = mvGameObjects.begin(); iter != mvGameObjects.end(); iter++)
	{
		iter->first->render();
	}
}

void NetworkManager::sendData(PacketTypes packet, int ID)
{
	bool destroyed = false;
	OutputMemoryBitStream MemStream;
	MemStream.Write(packet);

	switch (packet)
	{
	case CREATE_OBJECT:
	{
		MemStream.Write(mvGameObjects[ID].first->getInstance.handleEvent(GameEventType::CREATE_UNIT_EVENT));
		break;
	}
	case UPDATE_OBJECT:
	{
		MemStream.Write(mvGameObjects[ID].first->getInstance.update());
		break;
	}
	case DESTROY_OBJECT:
	{
		if (mvGameObjects.size() > 0)
		{
			std::vector<std::pair<Game*, int>>::iterator iter;
			for (iter = mvGameObjects.begin(); iter != mvGameObjects.end(); iter++)
			{
				mvGameObjects.erase(iter);
				destroyed = true;
				break;
			}
		}
		break;
	}
	default:
		return;
	}

	(*mpSocket)->Send(MemStream.GetBufferPtr(), MemStream.GetBitLength());

	if (destroyed)
	{
		mCurrentID--;
	}

}

void NetworkManager::receiveData()
{
	char buffer[4096];
	int32_t bytesReceived = (*mpSocket)->Receive(buffer, 4096);

	if (bytesReceived > 0)
	{
		InputMemoryBitStream MemStream = InputMemoryBitStream(buffer, 4096);
		PacketTypes recievePacketType;
		MemStream.Read(recievePacketType);

		int networkID;
		MemStream.Read(networkID);
		if (mCurrentID < networkID)
		{
			mCurrentID = networkID;
		}

		switch (recievePacketType)
		{
		case CREATE_OBJECT:
		{
			MemStream.Read(mvGameObjects[networkID].first->getInstance.handleEvent(GameEventType::CREATE_UNIT_EVENT));
			break;
		}
			
		case UPDATE_OBJECT:
		{
			if (mvGameObjects[networkID].first != nullptr)
			{
				mvGameObjects[networkID].first->getInstance()->update();
				break;
			}
			else
			{
				std::cout << "Nothing to update.\n";
			}

			break;
		}

		case DESTROY_OBJECT:
		{
			if (mvGameObjects.size() > 0)
			{
				std::vector<std::pair<Game*, int>>::iterator iter;
				for (iter = mvGameObjects.begin(); iter != mvGameObjects.end(); iter++)
				{
					mvGameObjects.erase(iter);
					mCurrentID--;
					break;
				}
			}
			break;
		}
		default:
			break;
		}
	}

	else if (bytesReceived <= -10035)
	{
		std::cout << "Connection terminated";
		exit(0);
	}
}
