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

	void init();
	void cleanup();

	~Networker();

	//Starting and connect to server
	bool initServer(std::string port);
	bool connect(std::string serverIpAddress, std::string port);

	//Update game state
	void getNewGameObjectState();
	void sendNewGameObjectState(int networkID, PacketType packetHeader);

	//Map
	void CleanupMap();
	void AddGameObjectToMap(GameObject* objectToAdd, int networkID);
	GameObject* GetGameObjectFromMap(int networkID) { return mGameObjectMap[networkID]; };
	map<int, GameObject*> GetMap() { return mGameObjectMap; };
	void UpdateMapObjects();
	void RenderMapObjects();

private:

	Networker();

	static Networker* mInstance;

	//Data
	TCPSocketPtr* mpTCPSocket;
	std::map<int, GameObject*> mGameObjectMap;
	//int mGameObjectID;
};
