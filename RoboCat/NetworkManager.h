#pragma once
#include "RoboCatPCH.h"
#include "../common/DeanLib/include/Trackable.h"
#include "Game.h"
#include "GameEvent.h"
#include "EventSystem.h"
#include "Unit.h"

enum PacketTypes
{
	CREATE_OBJECT,
	UPDATE_OBJECT,
	DESTROY_OBJECT
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
	~NetworkManager();

	bool initServer(std::string serverPort);
	bool connect(std::string clientIP, std::string clientPort);

	void createObject(Unit* obj, int objID);

	void sendData(PacketTypes packet, int ID, Unit* obj = nullptr);
	void receiveData();

private:

	Game* mpGame;
	TCPSocketPtr* mpSocket;
	static NetworkManager* mpsNetworkInstance;
	EventSystem* mpEventSystem;

	std::vector<std::pair<Unit*, int>> mvGameObjects;
	int mCurrentID;
};