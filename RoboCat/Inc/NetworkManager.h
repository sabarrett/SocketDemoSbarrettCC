#pragma once

#include <string>
#include "GameObjects.h"
#include "GameController.h"
#include "Colour.h"
#include "PlayerController.h"
#include <time.h>
#include "DeliveryNotificationManager.h"

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
	~NetworkManager();
	
	bool initServer(std::string port);
	void init(GraphicsLibrary* gLib, Colour p1Color, Colour p2Color, int dropOdds);
	bool connect(std::string serverIP, std::string port);

	void spawnObj(GameObjects* newObj, int networkID);
	void send(int networkID, TypePacket packetType);
	void recieve();

	void updateObj();
	void renderObj();

	int getCurrentID() { return mCurrentID; }
	void setCurrentID(int newID) { mCurrentID = newID; }

	float getTimeStamp() { return mPacketTimeStamp; }

	GameObjects* getObj() {};

private:

	static NetworkManager* mpNetworkInstance;
	GraphicsLibrary* mpGraphicsLib;
	TCPSocketPtr* mpTCPSocket;
	DeliveryNotificationManager* mpDeliveryNotifManager;

	float mPacketTimeStamp;

	std::vector<std::pair<GameObjects*, int>> mGameObjVector;

	Colour mP1Color, mP2Color;
	std::string mBubbleImgID;
	std::string mBoulderImgID;
	std::string mBeeImgID;

	int mCurrentID;
	int mDropOdds;

	bool mIsConnected;
};