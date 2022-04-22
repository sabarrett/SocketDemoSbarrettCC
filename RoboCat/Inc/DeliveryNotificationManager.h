#pragma once

#include "MemoryBitStream.h"
#include "InFlightPacket.h"
#include "AckRange.h"
#include <utility>
#include <deque>

using std::deque;

typedef uint16_t PacketSequenceNumber;

////https://www.geeksforgeeks.org/priority-queue-of-pairs-in-c-with-ordering-by-first-and-second-element/
//struct myComp {
//	constexpr bool operator()(
//		std::pair<InFlightPacket, OutputMemoryBitStream&> const& a,
//		std::pair<InFlightPacket, OutputMemoryBitStream&> const& b)
//		const noexcept
//	{
//		return a.first.GetSequenceNumber() > b.first.GetSequenceNumber();
//	}
//};

class Networker;

class DeliveryNotificationManager
{
public:
	
	DeliveryNotificationManager(bool inShouldSendAcks, bool inShouldProcessAcks, Networker* pNetworker);
	~DeliveryNotificationManager();
	
	inline	InFlightPacket* WriteState(OutputMemoryBitStream& inOutputStream);
	inline	bool ReadAndProcessState(InputMemoryBitStream& inInputStream);
	
	void ResendPacket(const int ID, const PacketSequenceNumber packetSequenceNum);
	void ProcessTimedOutPackets();
	
	uint32_t GetDroppedPacketCount() const { return mDroppedPacketCount; }
	uint32_t GetDeliveredPacketCount() const { return mDeliveredPacketCount; }
	uint32_t GetDispatchedPacketCount() const { return mDispatchedPacketCount; }
	
	//const deque<InFlightPacket>& GetInFlightPackets() const { return mInFlightPackets; }
	const deque<std::pair<InFlightPacket, OutputMemoryBitStream*>> GetInFlightPackets() const { return mInFlightPacketsPair; }
	
private:
		
	InFlightPacket* WriteSequenceNumber(OutputMemoryBitStream& inOutputStream);
	void WriteAckData(OutputMemoryBitStream& inOutputStream);
	
	//returns wether to drop the packet- if sequence number is too low!
	bool ProcessSequenceNumber(InputMemoryBitStream& inInputStream);
	void ProcessAcks(InputMemoryBitStream& inInputStream);
	
	void AddPendingAck(PacketSequenceNumber inSequenceNumber);
	void HandlePacketDeliveryFailure(const InFlightPacket& inFlightPacket, const PacketSequenceNumber packetSequenceNum);
	void HandlePacketDeliverySuccess(const InFlightPacket& inFlightPacket, const PacketSequenceNumber packetSequenceNum);
	
	PacketSequenceNumber mNextOutgoingSequenceNumber;
	PacketSequenceNumber mNextExpectedSequenceNumber;
	
	//deque<InFlightPacket> mInFlightPackets;
	deque<std::pair<InFlightPacket, OutputMemoryBitStream*>> mInFlightPacketsPair;
	deque<AckRange> mPendingAcks;
	
	bool mShouldSendAcks;
	bool mShouldProcessAcks;
	
	uint32_t mDeliveredPacketCount;
	uint32_t mDroppedPacketCount;
	uint32_t mDispatchedPacketCount;
	
	//Networker
	Networker* mpNetworker;
};



inline InFlightPacket* DeliveryNotificationManager::WriteState( OutputMemoryBitStream& inOutputStream )
{
	InFlightPacket* toRet = WriteSequenceNumber( inOutputStream );
	if( mShouldSendAcks )
	{
		WriteAckData( inOutputStream );
	}
	return toRet;
}

inline bool	DeliveryNotificationManager::ReadAndProcessState( InputMemoryBitStream& inInputStream )
{
	bool toRet = ProcessSequenceNumber( inInputStream );
	if( mShouldProcessAcks )
	{
		ProcessAcks( inInputStream );
	}
	return toRet;
}