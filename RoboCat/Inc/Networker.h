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
#include "RoboCatPCH.h"

enum PacketType
{
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

	void init(GraphicsLibrary* graphicsLibrary, std::string rockSpriteIdentifier, std::string playerSpriteIdentifier, float playerMoveSpeed, Colour wallColour, float arrivalTime);
	void cleanup();

	~Networker();

	//Starting and connect to server
	bool initServer(std::string port);
	bool connect(std::string serverIpAddress, std::string port);

	//Update game state
	PacketType receiveGameObjectState();
	void sendGameObjectState(int networkID, PacketType packetHeader);

	//Map
	void addGameObject(GameObject* objectToAdd, int networkID);
	GameObject* getGameObject(int networkID) { return mGameObjectsVec[networkID].second; };
	void updateGameObjects();
	void renderGameObjects();

	//Queue
	void SortPacketQueue();

private:

	Networker();

	static Networker* mInstance;

	//Data
	TCPSocketPtr* mpTCPSocket;
	std::vector<std::pair<int, GameObject*>> mGameObjectsVec;
	std::queue<std::pair<int, float>> mPacketQueue; //Might wanna change types depending on how we store the packets
	int mArrivalTime; 
	bool mIsInit;

	//Data for GameObject replication
	GraphicsLibrary* mpGraphicsLibrary;
	float mPlayerMoveSpeed;
	std::string mRockSpriteIdentifier;
	std::string mPlayerSpriteIdentifier;
	Colour mWallColour;
};