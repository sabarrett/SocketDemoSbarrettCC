#include "MyTData.h"

MyTdata::MyTdata(OutputMemoryBitStream& dataIn, TCPSocketPtr sendSocket)
{
	data = &dataIn;
	TCPSocket = sendSocket;
}

void MyTdata::HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
	TCPSocket->Send(data->GetBufferPtr(), data->GetBitLength());
	delete data;
}

void MyTdata::HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
	delete data;
}
