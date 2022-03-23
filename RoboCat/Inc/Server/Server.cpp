#include "Server/Server.h"
#include <iostream>
Server::Server()
{
}

Server::~Server()
{
}

void Server::Init()
{
	m_isRunning = true;
}

void Server::Update()
{
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		m_allObjects[i]->Update(m_keyStates[0]);
	}
}

GameObject* Server::CreateObject(std::string name, int UID)
{
	GameObject* gameObject = new GameObject;
	gameObject->Init("name", UID);
	int i = 0;
	m_allObjects.push_back(gameObject);
	return gameObject;
}

void Server::DestroyObject(GameObject* gameObject)
{
	gameObject->CleanUp();
}

void Server::CleanUp()
{
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		DestroyObject(m_allObjects[i]);
	}
}




//std::thread t1([&win, &quit]()
//	{
//		// Create listener socket
//		TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
//		if (listenSocket == nullptr)
//		{
//			SocketUtil::ReportError("Creating listening socket");
//			ExitProcess(1);
//		}
//		LOG("%s", "Listening socket created");
//
//		// Bind listener to address:port, trying other ports if 8080 is taken - allows for multiple unique instances on one device.
//		bool socketBound = false;
//		int port = 8080;
//		while (!socketBound && port <= 8090)
//		{
//			std::string IPAddr = StringUtils::Sprintf("0.0.0.0:%d", port);
//			SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString(IPAddr);
//			if (listenAddress == nullptr)
//			{
//				SocketUtil::ReportError("Creating listen address");
//			}
//			if (listenSocket->Bind(*listenAddress) != NO_ERROR)
//			{
//				SocketUtil::ReportError("Binding listening socket");
//				port++;
//			}
//			else
//			{
//				socketBound = true;
//				LOG("Bound listening socket to %s", IPAddr.c_str());
//			}
//		}
//
//		// Listen() - Listen on socket -> Non-blocking; tells OS we care about incoming connections on this socket
//		if (listenSocket->Listen() != NO_ERROR)
//		{
//			SocketUtil::ReportError("Listening on listening socket");
//			ExitProcess(1);
//		}
//		LOG("%s", "Waiting to accept connections...");
//
//		// Do non-blocking accept loop while running
//		listenSocket->SetNonBlockingMode(true);
//		SocketAddress incomingAddress;
//		TCPSocketPtr connSocket;
//		while (!quit)
//		{
//			connSocket = listenSocket->Accept(incomingAddress);
//			if (connSocket != nullptr)
//			{
//				Connection connection;
//				connSocket->SetNonBlockingMode(true);
//				connection.socketPtr = connSocket;
//				connection.connectionID = nextID;
//				nextID++;
//				connections.push_back(connection);
//				LOG("-------------Accepted connection from %s-------------", incomingAddress.ToString().c_str());
//				std::string msg = StringUtils::Sprintf("-------------Connected to %s!-------------", alias.c_str());
//				connSocket->Send(msg.c_str(), msg.length());
//			}
//		}
//		listenSocket->CleanUp();
//		return;
//	});