#pragma once
#include "Client/GameObject.h"
#include "Util/NetworkEnums.h"
#include "Util/RoboCatPCH.h"
#include "Util/NetworkManager.h"
#undef main

#include <string>
#include <vector>

class GameObject;

class Server : public NetworkManager {

public:
	Server(){};
	~Server() {};

	// Core functions
	int Init(int port);
	void Update();
	void CleanUp();

	GameObject* CreateObject(int type, uint8_t textureID);
	void DestroyObject(GameObject* gameObject);

	virtual void	ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress) override;
	virtual void	HandleConnectionReset(const SocketAddress& inFromAddress) override;

	// Network Simulation
	void SetLatency(float latency) { SetSimulatedLatency(latency); }
	void SetJitter(float jitter) { SetSimulatedJitter(jitter); }
	void SetDrop(float drop) { SetDropPacketChance(drop); }

	// Get
	bool Running() { return m_isRunning; };
private:
	void DoNetworking();
	void AcceptIncomingConnections();
	void SendWorldUpdatePackets();

	bool m_isRunning;
	int m_nextConnectionID = 0;

	std::vector<GameObject*> m_allObjects;

	
	// Networking
	struct Connection
	{
		Connection() :
			mDeliveryNotificationManager(false,true)
		{};
		TCPSocketPtr SocketPtrTCP;
		UDPSocketPtr SocketPtrUDP;
		SocketAddress socketAddress;
		int connectionID;
		DeliveryNotificationManager	mDeliveryNotificationManager;
	};
	std::vector<Connection*> m_connections;

	int m_nextUID = 0;
};