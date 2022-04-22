#pragma once

#include <string>
#include <map>
#include <stdlib.h>
#include <time.h>
#include <queue>

#include "GameObject.h"
#include "Rock.h"
#include "Wall.h"
#include "PlayerController.h"
#include "DeliveryNotificationManager.h"
#include "RoboCatPCH.h"

//https://www.geeksforgeeks.org/priority-queue-of-pairs-in-c-with-ordering-by-first-and-second-element/
struct myComp {
	constexpr bool operator()(
		std::pair<float, OutputMemoryBitStream*> const& a,
		std::pair<float, OutputMemoryBitStream*> const& b)
		const noexcept
	{
		return a.first < b.first;	//Sort by LEAST, since we send the queue.top and pop that first
	}
};

enum PacketType
{
	PACKET_HELLO,
	PACKET_CREATE,
	PACKET_UPDATE,
	PACKET_DELETE,
	PACKET_INVALID
};

//Networker is singleton (we only want one networker at a time)
class Networker
{
public:

	static Networker* GetInstance()
	{
		if (mInstance)
		{
			delete mInstance;
			mInstance = nullptr;
		}

		mInstance = new Networker;
		return mInstance;
	};

	void init(GraphicsLibrary* graphicsLibrary, std::string rockSpriteIdentifier, std::string playerSpriteIdentifier, float playerMoveSpeed, Colour wallColour/*, float arrivalTime*/);
	void cleanup();

	~Networker();

	//Starting and connect to server
	bool initServerUDP(std::string clientIpAddress, std::string servPort, std::string clientPort);
	bool connectUDP(std::string serverIpAddress, std::string servPort, std::string clientPort);

	//Update game state - UDP
	PacketType receiveGameObjectStateUDP();
	void sendGameObjectStateUDP(int networkID, PacketType packetHeader);
	void checkTimedOutPackets();

	//Map
	void addGameObject(/*GameObject**/ shared_ptr<GameObject> objectToAdd, int networkID);
	/*GameObject**/ shared_ptr<GameObject>  getGameObject(int networkID) { return mGameObjectsVec[networkID].second; };
	void updateGameObjects();
	void renderGameObjects();

private:

	Networker();
	static Networker* mInstance;

	//Data
	UDPSocketPtr* mpUDPSocket;
	SocketAddressPtr* mpSocketAddressPtr;
	std::vector<std::pair<int, /*GameObject**/ shared_ptr<GameObject>>> mGameObjectsVec;
	DeliveryNotificationManager* pDeliveryNotificationManager;
	std::priority_queue<std::pair<float, OutputMemoryBitStream*>, std::vector<std::pair<float, OutputMemoryBitStream*>>, myComp> mOutputBitStreamQueue;
	bool mbIsInitted;

	//Data for GameObject replication
	GraphicsLibrary* mpGraphicsLibrary;
	float mPlayerMoveSpeed;
	std::string mRockSpriteIdentifier;
	std::string mPlayerSpriteIdentifier;
	Colour mWallColour;
};