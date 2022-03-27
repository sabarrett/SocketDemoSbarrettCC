#include "Server/Server.h"
#include "Client/ArrayOfPointersGameObject.h"
#include <iostream>
#include <string>
Server::Server()
{
}

Server::~Server()
{
}

int Server::Init()
{
	m_isRunning = true;

	// Create listener socket
	m_listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (m_listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}

	// Bind listener to address:port, trying other ports if 8080 is taken - allows for multiple unique instances on one device.
	bool socketBound = false;
	int port = 64200;
	while (!socketBound && port <= 64300)
	{
		std::string IPAddr = StringUtils::Sprintf("0.0.0.0:%d", port);
		SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString(IPAddr);
		if (listenAddress == nullptr)
		{
			SocketUtil::ReportError("Creating listen address");
		}
		if (m_listenSocket->Bind(*listenAddress) != NO_ERROR)
		{
			SocketUtil::ReportError("Binding listening socket");
			port++;
		}
		else
		{
			socketBound = true;
			LOG("Bound Server TCP listening socket to %s", IPAddr.c_str());
		}
	}

	m_SocketPtrUDP = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	socketBound = false;
	while (!socketBound && port <= 64300)
	{
		std::string myIPAddr = StringUtils::Sprintf("%s:%d", "0.0.0.0", port);
		SocketAddressPtr clientUDPAddress = SocketAddressFactory::CreateIPv4FromString(myIPAddr);
		if (clientUDPAddress == nullptr)
		{
			SocketUtil::ReportError("Creating client address");
		}
		if (m_SocketPtrUDP->Bind(*clientUDPAddress) != NO_ERROR)
		{
			SocketUtil::ReportError("Binding UDP client socket");
			port++;
		}
		else
		{
			socketBound = true;
			LOG("Bound Server UDP socket to       %s", myIPAddr.c_str());
		}
		m_SocketPtrUDP->SetNonBlockingMode(true);
	}

	// Listen() - Listen on socket -> Non-blocking; tells OS we care about incoming connections on this socket
	if (m_listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on listening socket");
		ExitProcess(1);
	}
	LOG("%s", "--------------Server Done----------------");

	GameObject* object = CreateObject(1, 5);
	object->SetPosition(60, 60);


	return port;
}

void Server::Update()
{
	DoNetworking();

	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		m_allObjects[i]->Update(m_allObjects);
	}
}

void Server::DoNetworking()
{
	AcceptIncomingConnections();

	for (size_t i = 0; i < m_connections.size(); i++)
	{
		if (m_connections[i].SocketPtrTCP != nullptr)
		{
			char buffer[4096];
			int32_t bytesReceived = 1;
			while (bytesReceived > 0)
			{
				//bytesReceived = m_connections[i].SocketPtrTCP->Receive(buffer, 4096);
				SocketAddress fromAddr;
				bytesReceived = m_SocketPtrUDP->ReceiveFrom(buffer, 4096, fromAddr);

				InputMemoryBitStream inStream(buffer, bytesReceived);
				uint32_t packetType;
				inStream.Read(packetType);
				switch (packetType)
				{
				case PKTTYPE_MOVEOBJECT: // MOVE
					int targetID;
					int move;
					inStream.Read(targetID);
					inStream.Read(move);
					for (size_t j = 0; j < m_allObjects.size(); j++)
					{
						if (m_allObjects[j]->GetUID() == targetID)
						{
							m_allObjects[j]->AddVelocityFromInput(move);
						}
					}
					break;
				case PKTTYPE_CREATEOBJECT: // MOVE
					int textureID;
					int xPos;
					int yPos;
					inStream.Read(textureID);
					inStream.Read(xPos);
					inStream.Read(yPos);
					GameObject* object = CreateObject(0, textureID);
					object->SetPosition((float)xPos, (float)yPos);
					break;
				}
			}
		}
	}

	OutputMemoryBitStream outStream;
	outStream.Write(PKTTYPE_WORLDSTATE);
	outStream.Write(m_allObjects.size());
	for (size_t j = 0; j < m_allObjects.size(); j++)
	{
		outStream.Write(m_allObjects[j]->GetUID());
		m_allObjects[j]->Write(&outStream);
	}
	for (size_t i = 0; i < m_connections.size(); i++)
	{
		if (m_connections[i].SocketPtrTCP != nullptr)
		{
			m_SocketPtrUDP->SendTo(outStream.GetBufferPtr(), outStream.GetByteLength(), m_connections[i].socketAddress);
			//std::cout << "PKTSIZE: " << outStream.GetByteLength() << "\n";
		}
	}
}

void Server::AcceptIncomingConnections()
{
	// Do non-blocking accept loop while running
	m_listenSocket->SetNonBlockingMode(true);
	SocketAddress incomingAddress;
	TCPSocketPtr TCPconnSocket;
	UDPSocketPtr UDPconnSocket;
	TCPconnSocket = m_listenSocket->Accept(incomingAddress);
	if (TCPconnSocket != nullptr)
	{
		Connection connection;
		TCPconnSocket->SetNonBlockingMode(true);
		connection.SocketPtrTCP = TCPconnSocket;
		connection.socketAddress = incomingAddress;
		connection.connectionID = m_nextConnectionID;
		m_nextConnectionID++;
		m_connections.push_back(connection);
		//LOG("-------------Accepted connection from %s-------------", *incomingAddress->ToString().c_str());

		OutputMemoryBitStream outStream;
		outStream.Write(PKTTYPE_SETPLAYERID);
		outStream.Write(m_nextUID);
		TCPconnSocket->Send(outStream.GetBufferPtr(), outStream.GetByteLength());
		CreateObject(0, 0);
	}
}

GameObject* Server::CreateObject(int type, uint8_t textureID)
{
	if (type == 0)
	{
		GameObject* gameObject = new GameObject;
		gameObject->Init(m_nextUID, textureID);
		m_allObjects.push_back(gameObject);
		m_nextUID++;
		return gameObject;
	}
	if (type == 1)
	{
		ArrayOfPointersGameObject* gameObject = new ArrayOfPointersGameObject;
		gameObject->Init(m_nextUID, textureID);
		m_allObjects.push_back(gameObject);
		m_nextUID++;
		return gameObject;
	}
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

	for (size_t i = 0; i < m_connections.size(); i++)
	{
		if (m_connections[i].SocketPtrTCP != nullptr)
		{
			m_connections.erase(m_connections.begin() + i);
		}
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