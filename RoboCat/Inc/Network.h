#pragma once
#include "GameObjects.h"
#include "TCPSocket.h"
#include "MemoryBitStream.h"

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

	bool init(GraphicsSystems* graphicsSystem, std::string deanSprite, std::string amongSprite, std::string scottSprite, TCPSocketPtr liveSocket);
	void cleanUp();

	//void send(int networkID, PacketType packetTypeHead, GameObject* object);
	void send(PacketType packetTypeHead, GameObject* object);
	void receive();
	void draw();

private: 
	TCPSocketPtr mTCPSocket;
	std::vector<std::pair<int, GameObject*>> mGameObjects;
	//std::vector<GameObject*> mGameObjects;
	GraphicsSystems* mGraphicsSystem;
	std::string mDeanSprite;
	std::string mAmongSprite;
	std::string mScottSprite;
};