#include "RoboCatPCH.h"

void ReliableTransmissionData::HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
	(mTCPSocket)->Send(packetDataToResend->GetBufferPtr(), packetDataToResend->GetByteLength());
	//find a way to add to list if inflight pacvkets incase this one fails as well
}

void ReliableTransmissionData::HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const
{

}