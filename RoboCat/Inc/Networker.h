#pragma once

#include "Bullet.h"
#include "Effect.h"
#include "Player.h"
#include "GameData.h"

class Networker
{
public:
	Networker();
	~Networker();

	void SetSocket(TCPSocketPtr newSocket);

	GameObjectType ReceivePacket(int& currentMaxID, bool& isGameRunning);


private:
	
	TCPSocketPtr inSocket;
	GameData* gameData;
};