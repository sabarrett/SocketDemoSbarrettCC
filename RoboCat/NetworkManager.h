#pragma once
#include "RoboCatPCH.h"
#include "../common/DeanLib/include/Trackable.h"
#include "Game.h"
#include "GameEvent.h"
#include "EventSystem.h"
#include "Unit.h"
#include <stdlib.h>
#include <time.h>

enum PacketTypes
{
	CREATE_OBJECT,
	UPDATE_OBJECT,
	DESTROY_OBJECT,
	CONFIRM_PACKET
};

class NetworkManager : public Trackable
{
public:

	static NetworkManager* GetInstance()
	{
		if (mpsNetworkInstance)
		{
			delete mpsNetworkInstance;
			mpsNetworkInstance = nullptr;
		}
		mpsNetworkInstance = new NetworkManager;
		return mpsNetworkInstance;
	}

	NetworkManager();
	NetworkManager(int dropChance);
	~NetworkManager();

	bool initServer(std::string serverPort);
	bool connect(std::string clientIP, std::string clientPort);

	void createObject(Unit* obj, int objID);

	void sendData(PacketTypes packet, int ID, Unit* obj = nullptr);
	void receiveData();

	bool waitForConfirmation(int ID);
	void sendConfirmation(int ID);

private:

	Game* mpGame;
	TCPSocketPtr* mpSocket;
	static NetworkManager* mpsNetworkInstance;
	EventSystem* mpEventSystem;

	std::vector<std::pair<Unit*, int>> mvGameObjects;
	std::vector<std::pair<std::pair<const void*, size_t>, int>> mvPacketResendQueue;
	int mCurrentID;

	int mDropChance;
};