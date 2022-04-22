#pragma once

#include <unordered_map>

using std::unordered_map;

class DeliveryNotificationManager;

//in case we decide to change the type of the sequence number to use fewer or more bits
typedef uint16_t	PacketSequenceNumber;

class InFlightPacket
{
public:
	
	InFlightPacket( PacketSequenceNumber inSequenceNumber );
	
	PacketSequenceNumber GetSequenceNumber() const	{ return mSequenceNumber; }
	float				 GetTimeDispatched() const	{ return mTimeDispatched; }
	
	void 				 SetTransmissionData( int inKey, TransmissionData*	inTransmissionData )
	{
		mTransmissionDataMap[ inKey ] = inTransmissionData;
	}
	const TransmissionData* GetTransmissionData( int inKey ) const
	{
		auto it = mTransmissionDataMap.find( inKey );
		return ( it != mTransmissionDataMap.end() ) ? it->second : nullptr;
	}
	
	void			HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager, const PacketSequenceNumber packetSequenceNum) const;
	void			HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager, const PacketSequenceNumber packetSequenceNum) const;
	
private:
	PacketSequenceNumber	mSequenceNumber;
	float			mTimeDispatched;
	
	unordered_map< int, TransmissionData* >	mTransmissionDataMap;
};