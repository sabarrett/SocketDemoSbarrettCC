#pragma once

#include <memory>
using std::shared_ptr;

typedef uint16_t PacketSequenceNumber;

class DeliveryNotificationManager;

class TransmissionData
{
public:
	virtual void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager,  const PacketSequenceNumber packetSequenceNum) const = 0;
	virtual void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager,  const PacketSequenceNumber packetSequenceNum) const = 0;
};
typedef shared_ptr< TransmissionData > TransmissionDataPtr;