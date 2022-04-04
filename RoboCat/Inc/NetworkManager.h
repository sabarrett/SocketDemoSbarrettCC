//#include "RoboCatPCH.h"
#pragma once

#include <string>
#include "GameObjects.h"
#include "GameController.h"
#include "Colour.h"
#include "PlayerController.h"

enum TypePacket
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
	
	bool initServer(std::string port);
	void init(GraphicsLibrary* gLib, Colour p1Color, Colour p2Color);
	bool connect(std::string serverIP, std::string port);

	void spawnObj(GameObjects* newObj, int networkID);
	void send(int networkID, TypePacket packetType);
	void recieve();

	void updateObj();
	void renderObj();

	GameObjects* getObj() {};

private:

	static NetworkManager* mpNetworkInstance;
	GraphicsLibrary* mpGraphicsLib;
	TCPSocketPtr* mpTCPSocket;

	std::vector<std::pair<GameObjects*, int>> mGameObjVector;

	Colour mP1Color, mP2Color;
	std::string mBubbleImgID;
	std::string mBoulderImgID;
	std::string mBeeImgID;
};