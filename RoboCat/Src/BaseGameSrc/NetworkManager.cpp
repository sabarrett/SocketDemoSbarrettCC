#include "NetworkManager.h"

NetworkManager::NetworkManager()
{

}

NetworkManager::~NetworkManager()
{
	cleanup();
}

void NetworkManager::init(uint16_t port)
{
	mSocket = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	SocketAddress ownAddress(INADDR_ANY, port);
	mSocket->Bind(ownAddress);

	

	LOG("Initializing NetworkManager at port %d", port);

	mBytesReceivedPerSecond = 1.f;
	mBytesSentPerSecond =1.f;

}

void NetworkManager::cleanup()
{

}

void NetworkManager::SendPacket(const OutputMemoryBitStream& inOutputStream, const SocketAddress& inFromAddress)
{
	int sentByteCount = mSocket->SendTo(inOutputStream.GetBufferPtr(), inOutputStream.GetByteLength(), inFromAddress);
	if (sentByteCount > 0)
	{
		mBytesSentThisFrame += sentByteCount;
	}
}