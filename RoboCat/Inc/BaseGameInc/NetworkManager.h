#pragma once
#include "Trackable.h"
#include "RoboCatPCH.h"
class NetworkManager : public Trackable
{
public:

	static const uint32_t	kHelloCC = 'HELO';
	static const uint32_t	kWelcomeCC = 'WLCM';
	static const uint32_t	kStateCC = 'STAT';
	static const uint32_t	kInputCC = 'INPT';
	static const int		kMaxPacketsPerFrameCount = 10;

	NetworkManager();
	~NetworkManager();

	void init(uint16_t port);
	void cleanup();

	//void ProcessIncomingPackets();

	void SendPacket(const OutputMemoryBitStream& inOutputStream, const SocketAddress& inFromAddress);

private:
	
	class ReceivedPacket
	{
	public:
		//ReceivedPacket(float inReceivedTime, InputMemoryBitStream& inInputMemoryBitStream, const SocketAddress& inAddress);

		const	SocketAddress& GetFromAddress()	const { return mFromAddress; }
		float					GetReceivedTime()	const { return mReceivedTime; }
		InputMemoryBitStream& GetPacketBuffer() { return mPacketBuffer; }

	private:

		float					mReceivedTime;
		InputMemoryBitStream	mPacketBuffer;
		SocketAddress			mFromAddress;

	};

	queue< ReceivedPacket, list< ReceivedPacket > >	mPacketQueue;
	UDPSocketPtr mSocket;
	
	int mBytesSentThisFrame = 0;
	float mBytesReceivedPerSecond = 0.f;
	float mBytesSentPerSecond = 0.f;

};