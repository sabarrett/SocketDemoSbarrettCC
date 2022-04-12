#pragma once

#include "Bullet.h"
#include "Effect.h"
#include "Player.h"
#include "GameData.h"

static const int kMaxPacketsPerFrameCount = 10;

class Networker
{
public:
	Networker();
	~Networker();

	void SetSocket(TCPSocketPtr newSocket);
	DeliveryNotificationManager* pDeliveryNotificationManager = nullptr;
	//void SetFromAddress(SocketAddress newFromAddress);

	float GetRandomFloat();

	GameObjectType ReceivePacket(int& currentMaxID, bool& isGameRunning);
	void ReceivePacketIntoQueue();
	GameObjectType ProcessQueuedPackets(int& currentMaxID, bool& isGameRunning);

private:
	
	class ReceivedPacket
	{
	public:
		ReceivedPacket(float inReceivedTime, InputMemoryBitStream& inInputMemoryBitStream);

		float GetReceivedTime()	const { return mReceivedTime; }
		InputMemoryBitStream& GetPacketBuffer() { return mPacketBuffer; }

	private:

		float mReceivedTime;
		InputMemoryBitStream mPacketBuffer;
	};

	queue< ReceivedPacket, list< ReceivedPacket > >	mPacketQueue;

	TCPSocketPtr inSocket;
	GameData* gameData;


	float mDropPacketChance;
	float mSimulatedLatency;
};