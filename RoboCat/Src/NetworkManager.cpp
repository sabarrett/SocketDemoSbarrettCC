#include <RoboCatPCH.h>


void NetworkManager::Init(uint16_t port, const SocketAddress& toAddr)
{
	mToAddress = toAddr;
	mSocket = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	SocketAddress ownAddress(INADDR_ANY, port);
	mSocket->Bind(ownAddress);

	LOG("Initializing NetworkManager at port %d", port);


	//did we bind okay?
	if (mSocket == nullptr)
	{
		return;
	}

	if (mSocket->SetNonBlockingMode(true) != NO_ERROR)
	{
		return;
	}

	return;
}



void NetworkManager::ReceivePackets() {

}

void NetworkManager::SendPacket(const OutputMemoryBitStream& inOutputStream) {
	int sentByteCount = mSocket->SendTo(inOutputStream.GetBufferPtr(), inOutputStream.GetByteLength(), mToAddress);
}


void NetworkManager::ProcessPacket(InputMemoryBitStream& stream) {


}