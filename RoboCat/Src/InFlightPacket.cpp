#include "RoboCatPCH.h"
#include "InFlightPacket.h"

InFlightPacket::InFlightPacket( PacketSequenceNumber inSequenceNumber ) :
mSequenceNumber( inSequenceNumber ),
mTimeDispatched( Timing::sInstance.GetTimef() )
{
	//null out other transmision data params...
}


void InFlightPacket::HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager, const PacketSequenceNumber packetSequenceNum) const
{
	for( const auto& pair : mTransmissionDataMap )
	{
		pair.second->HandleDeliveryFailure(inDeliveryNotificationManager, packetSequenceNum);
	}
}

void InFlightPacket::HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager, const PacketSequenceNumber packetSequenceNum) const
{
	for( const auto& pair : mTransmissionDataMap )
	{
		pair.second->HandleDeliverySuccess(inDeliveryNotificationManager, packetSequenceNum);
	}
}