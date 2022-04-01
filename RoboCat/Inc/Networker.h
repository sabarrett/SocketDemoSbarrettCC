#pragma once

#include <string>
#include <map>

#include "GameObject.h"
#include "Rock.h"
#include "Wall.h"
#include "PlayerController.h"
#include "RoboCatPCH.h"

using std::map;

enum PacketType
{
	PACKET_CREATE,
	PACKET_UPDATE,
	PACKET_DELETE
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

	void init(GraphicsLibrary* graphicsLibrary, std::string rockSpriteIdentifier, std::string playerSpriteIdentifier, float playerMoveSpeed, Colour wallColour);
	void cleanup();

	~Networker();

	//Starting and connect to server
	bool initServer(std::string port);
	bool connect(std::string serverIpAddress, std::string port);

	//Update game state
	void receiveGameObjectState();
	void sendGameObjectState(int networkID, PacketType packetHeader);

	//Map
	void addGameObject(GameObject* objectToAdd, int networkID);
	GameObject* getGameObject(int networkID) { return mGameObjectsVec[networkID].second; };
	void updateGameObjects();
	void renderGameObjects();

private:

	Networker();

	static Networker* mInstance;

	//Data
	TCPSocketPtr* mpTCPSocket;
	std::vector<std::pair<int, GameObject*>> mGameObjectsVec;

	//Data for GameObject replication
	GraphicsLibrary* mpGraphicsLibrary;
	float mPlayerMoveSpeed;
	std::string mRockSpriteIdentifier;
	std::string mPlayerSpriteIdentifier;
	Colour mWallColour;
};