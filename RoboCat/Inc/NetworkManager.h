//#include "RoboCatPCH.h"
#pragma once

#include <string>
#include "GameObjects.h"
#include "GameController.h"
#include "Colour.h"

enum Packets
{
	PACKET_CREATE,
	PACKET_UPDATE,
	PACKET_DESTROY
};

class NetworkManager
{
public:
	static NetworkManager* GetInstance()
	{
		if (mpNetworkInstance)
		{
			delete mpNetworkInstance;
			mpNetworkInstance = nullptr;
		}

		mpNetworkInstance = new NetworkManager;
		return mpNetworkInstance;
	};

	NetworkManager();
	~NetworkManager() {};
	void init(GraphicsLibrary* gLib, Colour p1Color, Colour p2Color);
	
	bool initServer(std::string port);
	bool connect(std::string serverIP, std::string port);

	void recieve();
	void send(int networkID, Packets packetType);

	void spawnObj(GameObjects* newObj, int networkID);
	void updateObj();
	void renderObj();
	GameObjects* getObj() {};

private:

	static NetworkManager* mpNetworkInstance;
	GraphicsLibrary* mpGraphicsLib;
	TCPSocketPtr* mpTCPSocket;

	Colour mP1Color, mP2Color;

	std::vector<std::pair<GameObjects*, int>> mObjectsVector;
	std::string mBubbleImgID;
	std::string mBoulderImgID;
	std::string mBeeImgID;

	
};