#pragma once
#include "RoboCatPCH.h"

class TransmissionData;
class DeliveryNotificationManager;
class Game;

class MyTdata : public TransmissionData
{
public:

	MyTdata(OutputMemoryBitStream& dataIn, TCPSocketPtr sendSocket);

	void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager);
	void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager);

private:
	OutputMemoryBitStream data;
	TCPSocketPtr TCPSocket;
};
