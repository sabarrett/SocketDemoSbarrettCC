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

//struct GamePacket
//{
//	char* buffer;
//	int32_t byteRecieve;
//	float dispatchTime;
//};

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
	//bool initServer(std::string port);
	//bool connect(std::string serverIpAddress, std::string port);

	bool initServerUDP(std::string serverIpAddress, std::string port);
	bool connectUDP(std::string serverIpAddress, std::string port);

	//Update game state
	//PacketType receiveGameObjectState();
	//void sendGameObjectState(int networkID, PacketType packetHeader);

	//Update game state - UDP
	PacketType receiveGameObjectStateUDP();
	void sendGameObjectStateUDP(int networkID, PacketType packetHeader);
	void checkTimedOutPackets();
	//PacketType processPacket(GamePacket gamePacket);

	//Map
	void addGameObject(GameObject* objectToAdd, int networkID);
	GameObject* getGameObject(int networkID) { return mGameObjectsVec[networkID].second; };
	void updateGameObjects();
	void renderGameObjects();

	////Queue
	//void sortPacketQueue();
	//int findMinIndex(int sortedIndex);
	//void insertMinIndexToEnd(int minIndex);

private:

	Networker();

	static Networker* mInstance;

	//Data
	//TCPSocketPtr* mpTCPSocket;
	UDPSocketPtr* mpUDPSocket;
	SocketAddressPtr* mpSocketAddressPtr;
	std::vector<std::pair<int, GameObject*>> mGameObjectsVec;
	//std::queue<InFlightPacket*> mInFlightPacketsQueue;
	DeliveryNotificationManager* pDeliveryNotificationManager;
	std::priority_queue<std::pair<float, OutputMemoryBitStream*>, std::vector<std::pair<float, OutputMemoryBitStream*>>, myComp> mOutputBitStreamQueue;
	//int mArrivalTime;
	bool mbIsInitted;

	//Data for GameObject replication
	GraphicsLibrary* mpGraphicsLibrary;
	float mPlayerMoveSpeed;
	std::string mRockSpriteIdentifier;
	std::string mPlayerSpriteIdentifier;
	Colour mWallColour;
};