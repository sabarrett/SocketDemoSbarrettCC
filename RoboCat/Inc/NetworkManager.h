#include "MemoryBitStream.h"

class NetworkManager {
private:

	UDPSocketPtr	mSocket;
	SocketAddress mToAddress;

public:
	void Init(uint16_t port, const SocketAddress& toAddr);
	

	void ReceivePackets();

	void SendPacket(const OutputMemoryBitStream& inOutputStream);


	void ProcessPacket(InputMemoryBitStream& stream);


};