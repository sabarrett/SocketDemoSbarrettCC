#pragma once
#include "../common/DeanLib/include/Trackable.h"
#include "Game.h"
#include "RoboCatPCH.h"

enum PacketTypes
{
	CREATE_OBJECT,
	UPDATE_OBJECT,
	DESTROY_OBJECT
};

class NetworkManager : public Trackable
{
public:
	NetworkManager();
	~NetworkManager();

	void initServer(std::string serverPort);
	void connect(std::string serverPort, std::string clientPort);

	void createObject();
	void updateObject();
	void destroyObject();
	void renderObject();

	void sendData();
	void receiveData();

private:

	Game* mpGame;
	TCPSocket* mpSocket;
};