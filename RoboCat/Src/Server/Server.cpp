#include "Server/Server.h"
#include "Client/ColorChangerGameObject.h"
#include "Client/CollectableGameObject.h"

#include <iostream>
#include <string>

int Server::Init(int newPort)
{
	// Create listener socket
	m_socketPtrTCP = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (m_socketPtrTCP == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}

	// Bind listener to address:port, trying other ports if 64200 is taken - allows for multiple unique instances on one device.
	bool socketBound = false;
	//int port = 64200;
	int port = newPort;
	while (!socketBound && port <= 64300)
	{
		std::string IPAddr = StringUtils::Sprintf("0.0.0.0:%d", port);
		SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString(IPAddr);
		if (listenAddress == nullptr)
		{
			SocketUtil::ReportError("Creating listen address");
		}
		if (m_socketPtrTCP->Bind(*listenAddress) != NO_ERROR)
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

	m_socketPtrUDP = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	socketBound = false;
	while (!socketBound && port <= 64300)
	{
		std::string myIPAddr = StringUtils::Sprintf("%s:%d", "0.0.0.0", port);
		SocketAddressPtr clientUDPAddress = SocketAddressFactory::CreateIPv4FromString(myIPAddr);
		if (clientUDPAddress == nullptr)
		{
			SocketUtil::ReportError("Creating client address");
		}
		if (m_socketPtrUDP->Bind(*clientUDPAddress) != NO_ERROR)
		{
			SocketUtil::ReportError("Binding UDP client socket");
			port++;
		}
		else
		{
			socketBound = true;
			LOG("Bound Server UDP socket to       %s", myIPAddr.c_str());
		}
		m_socketPtrUDP->SetNonBlockingMode(true);
	}

	// Listen() - Listen on socket -> Non-blocking; tells OS we care about incoming connections on this socket
	if (m_socketPtrTCP->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on listening socket");
		ExitProcess(1);
	}
	LOG("%s", "--------------Server Done----------------");

	GameObject* object = CreateObject(1, 5);
	object->SetPosition(800, 450);

	m_isRunning = true;

	return port;
}

void Server::Update()
{
	DoNetworking();

	// Update all objects
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		m_allObjects[i]->Update(m_allObjects);
	}

	// Destroy want-to-die objects
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		if (m_allObjects[i]->GetShouldDestroy())
		{
			m_allObjects[i]->CleanUp();
			m_allObjects.erase(m_allObjects.begin() + i);
		}
	}
}

void Server::DoNetworking()
{
	AcceptIncomingConnections();

	ProcessIncomingPackets();

	SendWorldUpdatePackets();
}

void Server::AcceptIncomingConnections()
{
	// Do non-blocking accept loop while running
	m_socketPtrTCP->SetNonBlockingMode(true);
	SocketAddress incomingAddress;
	TCPSocketPtr TCPconnSocket;
	TCPconnSocket = m_socketPtrTCP->Accept(incomingAddress);
	if (TCPconnSocket != nullptr)
	{
		Connection* connection = new Connection;
		TCPconnSocket->SetNonBlockingMode(true);
		connection->SocketPtrTCP = TCPconnSocket;
		connection->socketAddress = incomingAddress;
		connection->connectionID = m_nextConnectionID;
		m_nextConnectionID++;
		m_connections.push_back(connection);
		//LOG("-------------Accepted connection from %s-------------", incomingAddress->ToString().c_str());

		OutputMemoryBitStream outStream;
		outStream.Write(PKTTYPE_SETPLAYERID);
		outStream.Write(m_nextUID);
		TCPconnSocket->Send(outStream.GetBufferPtr(), outStream.GetByteLength());
		CreateObject(0, 0);

		for (size_t j = 0; j < m_allObjects.size(); j++)
		{
			m_allObjects[j]->ForceSendUpdate();
		}
	}
}

void Server::ProcessPacket(InputMemoryBitStream& inStream, const SocketAddress& inFromAddress)
{
	SocketAddress fromAddress = inFromAddress; // <-- Declaring to hide an unreference warning from compiler - Also keeping inFromAddress because its useful.
	for (size_t i = 0; i < m_connections.size(); i++)
	{
		if (m_connections[i]->socketAddress == fromAddress)
		{
			uint32_t packetType;
			inStream.Read(packetType);
			switch (packetType)
			{
			case PKTTYPE_MOVEOBJECT:
				if (m_connections[i]->mDeliveryNotificationManager.ReadAndProcessState(inStream))
				{
					int targetID;
					uint8_t move;
					inStream.Read(targetID);
					inStream.Read(move);
					for (size_t j = 0; j < m_allObjects.size(); j++)
					{
						if (m_allObjects[j]->GetUID() == targetID)
						{
							m_allObjects[j]->AddVelocityFromInput(move);
						}
					}
				}
				break;
			case PKTTYPE_CREATEOBJECT:
				if (m_connections[i]->mDeliveryNotificationManager.ReadAndProcessState(inStream))
				{
					uint8_t textureID;
					int xPos;
					int yPos;
					inStream.Read(textureID);
					inStream.Read(xPos);
					inStream.Read(yPos);
					GameObject* object = CreateObject(GAMEOBJECT_COLLECTABLE, textureID);
					object->SetPosition((float)xPos, (float)yPos);
				}
				break;
		}
		}
	}
}

void Server::HandleConnectionReset(const SocketAddress& inFromAddress)
{
	for (size_t i = 0; i < m_connections.size(); i++)
	{
		if (m_connections[i]->socketAddress == inFromAddress)
		{
			m_connections.erase(m_connections.begin() + i);
		}
	}
}

void Server::SendWorldUpdatePackets()
{
	// Write worldstate content
	OutputMemoryBitStream worldStateContent;
	worldStateContent.Write(m_allObjects.size());
	for (size_t j = 0; j < m_allObjects.size(); j++)
	{
		worldStateContent.Write(m_allObjects[j]->GetUID());
		worldStateContent.Write(m_allObjects[j]->GetType());
		worldStateContent.Write(m_allObjects[j]->GetContainsUpdatedInfo());
		m_allObjects[j]->Write(&worldStateContent);
	}

	// Write a header, append the worldstate, and send
	for (size_t i = 0; i < m_connections.size(); i++)
	{
		if (m_connections[i]->SocketPtrTCP != nullptr)
		{
			OutputMemoryBitStream packet;
			packet.Write(PKTTYPE_WORLDSTATE);
			m_connections[i]->mDeliveryNotificationManager.WriteState(packet);
			packet.WriteBytes(worldStateContent.GetBufferPtr(), worldStateContent.GetByteLength());
			SendPacket(packet, m_connections[i]->socketAddress);
		}
	}
}

GameObject* Server::CreateObject(int type, uint8_t textureID)
{
	GameObject* gameObject;
	switch (type)
	{
	case GAMEOBJECT_BASE:
		gameObject = new GameObject;
		break;
	case GAMEOBJECT_COLORCHANGER:
		gameObject = new ColorChangerGameObject;
		break;
	case GAMEOBJECT_COLLECTABLE:
		gameObject = new CollectableGameObject;
		break;
	default:
		gameObject = new GameObject;
		break;
	}
	gameObject->Init(m_nextUID, textureID);
	m_allObjects.push_back(gameObject);
	m_nextUID++;
	return gameObject;
}

void Server::DestroyObject(GameObject* gameObject)
{
	gameObject->CleanUp();
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		if (m_allObjects[i] == gameObject)
		{
			m_allObjects.erase(m_allObjects.begin() + i);
		}
	}
}

void Server::CleanUp()
{
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		DestroyObject(m_allObjects[i]);
		m_allObjects.erase(m_allObjects.begin() + i);
	}

	for (size_t i = 0; i < m_connections.size(); i++)
	{
		if (m_connections[i]->SocketPtrTCP != nullptr)
		{
			m_connections.erase(m_connections.begin() + i);
		}
	}
}