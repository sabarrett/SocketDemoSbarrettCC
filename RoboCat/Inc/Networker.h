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

	static Networker *GetInstance()
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
	void getNewGameObjectState();
	void sendNewGameObjectState(int networkID, PacketType packetHeader);

	//Map
	void addGameObjectToMap(GameObject* objectToAdd, int networkID);
	GameObject* getGameObjectFromMap(int networkID) { return mGameObjectMap[networkID]; };
	map<int, GameObject*> getMap() { return mGameObjectMap; };
	void updateMapObjects();
	void renderMapObjects();

private:

	Networker();

	static Networker* mInstance;

	//Data
	TCPSocketPtr* mpTCPSocket;
	std::map<int, GameObject*> mGameObjectMap;

	//Data for GameObject replication
	GraphicsLibrary* mpGraphicsLibrary;
	float mPlayerMoveSpeed;
	std::string mRockSpriteIdentifier;
	std::string mPlayerSpriteIdentifier;
	Colour mWallColour;
};
