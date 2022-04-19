#pragma once
#include "RoboCatPCH.h"

class DeliveryNotificationManager;
class Game;

class MyTdata : public TransmissionData
{
public:

	MyTdata(OutputMemoryBitStream& dataIn, TCPSocketPtr sendSocket);

	void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const override;
	void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const override;

private:
	OutputMemoryBitStream data;
	TCPSocketPtr TCPSocket;
};
