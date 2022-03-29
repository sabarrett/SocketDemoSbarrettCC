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

	~Networker();

	//Starting and connect to server
	void InitServer(std::string port);
	void Connect(std::string clientIpAddress, std::string serverIpAddress, std::string port);

	//Update game state
	void GetNewGameObjectState(map<int, GameObject*> gameObjectMap);
	void SendNewGameObjectState(map<int, GameObject*> gameObjectMap, int ID);

private:

	Networker();
	static Networker* mInstance;

	//Data
	TCPSocketPtr mTCPSocket;
};
