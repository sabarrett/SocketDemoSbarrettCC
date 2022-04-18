#pragma once
class DeliveryNotificationManager;

class TransmissionData
{
public:
	virtual void HandleDeliveryFailure( DeliveryNotificationManager* inDeliveryNotificationManager ) const = 0;
	virtual void HandleDeliverySuccess( DeliveryNotificationManager* inDeliveryNotificationManager ) const = 0;
};
typedef shared_ptr< TransmissionData > TransmissionDataPtr;

class ReliableTransmissionData : public TransmissionData
{
public:
	ReliableTransmissionData(std::shared_ptr<OutputMemoryBitStream> packetDataToSave, TCPSocketPtr socket) { packetDataToResend = packetDataToSave; mTCPSocket = socket; };
	virtual void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const;
	virtual void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const;
private:
	std::shared_ptr<OutputMemoryBitStream> packetDataToResend;
	TCPSocketPtr mTCPSocket;
};