#include "MyTData.h"

MyTdata::MyTdata(OutputMemoryBitStream& dataIn, TCPSocketPtr sendSocket) : data(dataIn)
{
	TCPSocket = sendSocket;
}

void MyTdata::HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) 
{

}

void MyTdata::HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) 
{
	TCPSocket->Send(data.GetBufferPtr(), data.GetBitLength());
}
