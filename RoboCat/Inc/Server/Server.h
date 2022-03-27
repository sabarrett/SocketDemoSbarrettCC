#pragma once

class GameObject;
#include "Client/GameObject.h"
#include "Util/NetworkEnums.h"
#include "Util/RoboCatPCH.h"
#include "SDL.h"
#include "SDL_image.h"
#undef main

#include <string>
#include <vector>


class Server {

public:
	Server();
	~Server();

	// Core functions
	int Init();
	void Update();
	
	void CleanUp();

	

	GameObject* CreateObject(int type, uint8_t textureID);
	void DestroyObject(GameObject* gameObject);

	// Get
	bool Running() { return m_isRunning; };
	const Uint8** getKeyStates() { return m_keyStates; }
private:
	void DoNetworking();
	void AcceptIncomingConnections();

	bool m_isRunning;
	int m_nextConnectionID = 0;

	std::vector<GameObject*> m_allObjects;

	const Uint8* m_keyStates[8];


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