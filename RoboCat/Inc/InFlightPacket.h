#pragma once
#include <set>

class DeliveryNotificationManager;

//in case we decide to change the type of the sequence number to use fewer or more bits
typedef uint16_t	PacketSequenceNumber;

class InFlightPacket
{
public:
	
	InFlightPacket( PacketSequenceNumber inSequenceNumber );
	
	PacketSequenceNumber GetSequenceNumber() const	{ return mSequenceNumber; }
	float				 GetTimeDispatched() const	{ return mTimeDispatched; }
	
	void 				 SetTransmissionData( int inKey, TransmissionDataPtr	inTransmissionData )
	{
		mTransmissionDataMap.insert(inTransmissionData);
	}
	
	void			HandleDeliveryFailure( DeliveryNotificationManager* inDeliveryNotificationManager ) const;
	void			HandleDeliverySuccess( DeliveryNotificationManager* inDeliveryNotificationManager ) const;
	
private:
	PacketSequenceNumber	mSequenceNumber;
	float			mTimeDispatched;
	
	std::set<TransmissionDataPtr>	mTransmissionDataMap;
};