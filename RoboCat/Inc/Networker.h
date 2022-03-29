#pragma once

#include <string>
#include <map>

#include "GameObject.h"
#include "Rock.h"
#include "Wall.h"
#include "PlayerController.h"
#include "RoboCatPCH.h"

using std::string;
using std::map;

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

	void cleanup();

	~Networker();

	//Starting and connect to server
	void initServer(std::string port);
	void connect(std::string clientIpAddress, std::string serverIpAddress, std::string port);

	//Update game state
	void getNewGameObjectState(map<int, GameObject*> gameObjectMap);
	void sendNewGameObjectState(map<int, GameObject*> gameObjectMap, int ID);

private:

	Networker();
	static Networker* mInstance;

	//Data
	TCPSocketPtr mTCPSocket;
};
