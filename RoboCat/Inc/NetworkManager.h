#pragma once

#include <string>
#include "GameObjects.h"
#include "GameController.h"
#include "Colour.h"
#include "PlayerController.h"
#include <time.h>
#include "DeliveryNotificationManager.h"
#include <queue>
#include "GraphicsLibrary.h"

enum TypePacket
{
	PACKET_CREATE,
	PACKET_UPDATE,
	PACKET_DESTROY,
	PACKET_INIT,
	CONFIRM
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

	bool mIsConnected;

	NetworkManager();
	~NetworkManager();
	
	bool initServer(std::string port);
	void init(GraphicsLibrary* gLib, Colour p1Color, Colour p2Color, int dropOdds);
	bool connect(std::string serverIP, std::string port);

	void spawnObj(GameObjects* newObj, int networkID);
	void send(int networkID, TypePacket packetType);
	void recieve();
	
	PlayerController* recieveInitFromServer(/*PlayerController** playerServer, */GraphicsLibrary* gLib);

	void updateObj();
	void renderObj();

	void update(float deltaTime, float time);

	int getCurrentID() { return mServerNetworkID; }
	void setCurrentID(int newID) { mServerNetworkID = newID; }

	float getTimeStamp() { return mPacketTimeStamp; }

	void createConfirmPacket(int ID);
	bool waitForConfirmPacket(int ID);

	void processPacket(KeyCode key, string img, int player);
	void requestPacket(KeyCode key);

	void setServerRole(bool role) { mIsServer = role; }
	bool getServerRole() { return mIsServer; }

	GameObjects* getObj() {};

private:

	static NetworkManager* mpNetworkInstance;
	GraphicsLibrary* mpGraphicsLib;
	TCPSocketPtr* mpTCPSocket;
	DeliveryNotificationManager* mpDeliveryNotifManager;

	float mPacketTimeStamp;

	std::vector<std::pair<GameObjects*, int>> mGameObjVector;
	//std::vector<std::pair<std::pair<const void*, size_t>, int>> mPendingResendPackets;
	std::vector<std::pair<OutputMemoryBitStream*, int>> mPendingResendPackets;

	Colour mP1Color, mP2Color;
	std::string mBubbleImgID;
	std::string mBoulderImgID;
	std::string mBeeImgID;

	int mServerNetworkID = 0;
	int mDropOdds;
	int mLastSentID;
	float mTimeTillResend;

	int mClientPacketID = 0;

	bool mIsServer;

	GraphicsLibrary* pGraphicsLib;
	float screenSizeX = 1000.0;
	float screenSizeY = 700.0;
};