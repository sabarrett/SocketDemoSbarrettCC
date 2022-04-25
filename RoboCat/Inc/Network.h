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

enum class PacketType
{
	CREATE_PACKET,
	DELETE_PACKET
};

class Network
{
public :
	Network(GraphicsSystems* graphicsSystem, DeliveryNotificationManager* deliveryManager, std::string deanSprite, std::string amongSprite, std::string scottSprite, TCPSocketPtr liveSocket);
	~Network();

	void Send(PacketType packetTypeHead, GameObject* object);
	void Receive();
	void Draw();
	void ProcessQueuedPackets();
	void ProcessPacket(InputMemoryBitStream inputBitStream);

	std::vector<std::pair<int, GameObject*>> getmGameObjects() { return mGameObjects; };

private: 
	TCPSocketPtr mTCPSocket;
	std::vector<std::pair<int, GameObject*>> mGameObjects;
	GraphicsSystems* mGraphicsSystem;
	DeliveryNotificationManager* mDeliverymanager;
	ReliableTransmissionData* mTData;

	std::string mDeanSprite;
	std::string mAmongSprite;
	std::string mScottSprite;

	int mDropPacketChance = 25;
	int mSimulatedLatency = 250;
	int mDoubleSimulatedMaxJitter = 250;
	std::vector<std::pair<float, InputMemoryBitStream>> mPacketList;

};