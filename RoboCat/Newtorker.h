#pragma once

#include <string>

#include "GameObject.h"
#include "RoboCatPCH.h"

using std::string;

//Networker is singleton (we only want one networker at a time)
class Networker
{
public:

	static Networker* GetInstance();

	virtual ~Networker();

	//Starting and connect to server
	void InitServer();
	void Connect(string IPAddress);

	//Update game state
	void UpdateGameObject(GameObject gameObject);

private:

	Networker();
	static Networker* mInstance;

	//Data
	TCPSocketPtr mTCPSocket;
};
