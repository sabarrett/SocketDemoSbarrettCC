#pragma once
#include "GameObjects.h"
#include "TCPSocket.h"
#include "MemoryBitStream.h"
#include "RoboCatPCH.h"

#include <vector>
#include <string>
#include <map>
#include <utility>

using std::vector;
using std::pair;
using std::string;
using std::map;

enum PacketType
{
	CREATE_PACKET,
	DELETE_PACKET
};

class Network
{
public :
	Network() {};

	bool init(GraphicsSystems* graphicsSystem, DeliveryNotificationManager* deliveryManager, std::string deanSprite, std::string amongSprite, std::string scottSprite, TCPSocketPtr liveSocket);
	void cleanUp();

	void send(PacketType packetTypeHead, GameObject* object);
	PacketType receive();
	void draw();

	std::vector<std::pair<int, GameObject*>> getmGameObjects() { return mGameObjects; };

private: 
	TCPSocketPtr mTCPSocket;
	std::vector<std::pair<int, GameObject*>> mGameObjects;
	GraphicsSystems* mGraphicsSystem;
	DeliveryNotificationManager* mDeliverymanager;

	std::string mDeanSprite;
	std::string mAmongSprite;
	std::string mScottSprite;
};