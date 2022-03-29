#pragma once
#include "Client/GameObject.h"
#include "Util/NetworkEnums.h"
#include "Util/RoboCatPCH.h"
#undef main

#include <string>
#include <vector>

class GameObject;

class Server {

public:
	Server() {};
	~Server() {};

	// Core functions
	int Init();
	void Update();
	void CleanUp();

	GameObject* CreateObject(int type, uint8_t textureID);
	void DestroyObject(GameObject* gameObject);

	// Get
	bool Running() { return m_isRunning; };
private:
	void DoNetworking();
	void AcceptIncomingConnections();
	void HandleIncomingPackets();
	void SendWorldUpdatePackets();

	bool m_isRunning;
	int m_nextConnectionID = 0;

	std::vector<GameObject*> m_allObjects;

	// Networking
	struct Connection
	{
		TCPSocketPtr SocketPtrTCP;
		UDPSocketPtr SocketPtrUDP;
		SocketAddress socketAddress;
		int connectionID;
	};
	TCPSocketPtr m_listenSocket;
	UDPSocketPtr m_SocketPtrUDP;
	std::vector<Connection> m_connections;

	int m_nextUID = 0;
};