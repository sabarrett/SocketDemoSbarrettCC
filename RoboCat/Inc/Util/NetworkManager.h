#pragma once
#include "Util/RoboCatPCH.h"
#include "Util/NetworkEnums.h"

typedef unordered_map< int, GameObject* > IntToGameObjectMap;

class NetworkManager
{
public:
	static const int		kMaxPacketsPerFrameCount = 10;

	NetworkManager();
	virtual ~NetworkManager();

	bool	Init();
	void	ProcessIncomingPackets();

	virtual void	ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress) = 0;
	virtual void	HandleConnectionReset(const SocketAddress& inFromAddress) { (void)inFromAddress; }

	void	SendPacket(OutputMemoryBitStream& inOutputStream, const SocketAddress& inFromAddress);

	const WeightedTimedMovingAverage& GetBytesReceivedPerSecond()	const { return mBytesReceivedPerSecond; }
	const WeightedTimedMovingAverage& GetBytesSentPerSecond()		const { return mBytesSentPerSecond; }

	void	SetDropPacketChance(float inChance) { m_dropPacketChance = inChance; }
	void	SetSimulatedLatency(float inLatency) { m_simulatedLatency = inLatency; }
	void	SetSimulatedJitter(float inJitter) { m_jitter = inJitter; }

protected:

	std::vector<GameObject*> m_allObjects;

	GameObject* CreateObject(int type, int UID, uint8_t textureID);
	GameObject* GetObjectByUID(int UID);
	void DestroyObject(GameObject* gameObject);
	bool CheckIfObjectExistsWithUID(int UID);

private:

	class ReceivedPacket
	{
	public:
		ReceivedPacket(float inReceivedTime, InputMemoryBitStream& inInputMemoryBitStream, const SocketAddress& inAddress);

		const	SocketAddress& GetFromAddress()	const { return mFromAddress; }
		float					GetReceivedTime()	const { return mReceivedTime; }
		InputMemoryBitStream& GetPacketBuffer() { return mPacketBuffer; }

	private:

		float					mReceivedTime;
		InputMemoryBitStream	mPacketBuffer;
		SocketAddress			mFromAddress;

	};
	class SentPacket
	{
	public:
		SentPacket(float inSentTime, OutputMemoryBitStream& OutputMemoryBitStream, const SocketAddress& inFromAddress);

		const	SocketAddress& GetFromAddress()	const { return mToAddress; }
		float					GetSentTime()	const { return mSentTime; }
		OutputMemoryBitStream GetPacketBuffer() { return mPacketBuffer; }
		int					sentFramesAgo;
	private:

		float					mSentTime;
		OutputMemoryBitStream	mPacketBuffer;
		SocketAddress			mToAddress;
	};
	
	void	ProcessQueuedPackets();

	WeightedTimedMovingAverage	mBytesReceivedPerSecond;
	WeightedTimedMovingAverage	mBytesSentPerSecond;

	int							mBytesSentThisFrame;

	float						m_dropPacketChance;
	float						m_simulatedLatency;
	float						m_jitter;

protected:
	virtual void	ReadIncomingPacketsIntoQueue();

	queue< ReceivedPacket, list< ReceivedPacket > >	mPacketQueue;
	vector<SentPacket*>	mSentPacketsList;

	TCPSocketPtr m_socketPtrTCP;
	UDPSocketPtr m_socketPtrUDP;
};
