
#include "RoboCatPCH.h"


void PacketDestroy::Read(InputMemoryBitStream& bitstream) {
	bitstream.Read(X);
	bitstream.Read(Y);
}
void PacketDestroy::Write(OutputMemoryBitStream& bitstream) {
	bitstream.Write(X);
	bitstream.Write(Y);
}

void PacketMove::Read(InputMemoryBitStream& bitstream) {
	bitstream.Read(objectID);
	bitstream.Read(position);
	bitstream.Read(velocity);
}

void PacketMove::Write(OutputMemoryBitStream& bitstream) {
	bitstream.Write(objectID);
	bitstream.Write(position);
	bitstream.Write(velocity);
}

void PacketPlayerInfo::Read(InputMemoryBitStream& bitstream) {
	bitstream.Read(nickname);
}

void PacketPlayerInfo::Write(OutputMemoryBitStream& bitstream) {
	bitstream.Write(nickname);
}

void PacketManager::HandlePacket_Destroy(InputMemoryBitStream& bitstream) {

}
void PacketManager::HandlePacket_Move(InputMemoryBitStream& bitstream) {

}
void PacketManager::HandlePacket_PlayerInfo(InputMemoryBitStream& bitstream) {

}


void PacketManager::SendPacket(Packet_Destroy pkt) {
	OutputMemoryBitStream writestream;
	writestream.Write(pkt.X);
	writestream.Write(pkt.Y);

	//Send(writestream);
}

void PacketManager::SendPacket(Packet_Move pkt) {
	OutputMemoryBitStream writestream;
	writestream.Write(pkt.objectID);
	writestream.Write(pkt.position);
	writestream.Write(pkt.velocity);
}

void PacketManager::SendPacket(Packet_PlayerInfo pkt) {
	OutputMemoryBitStream writestream;
	writestream.Write(pkt.nickname);
}

bool PacketManager::HandleInput(UDPSocketPtr socket, SocketAddress& out_addr) {


	char buffer[2048];
	int byteCount = socket->ReceiveFrom(buffer, 2048, out_addr);
	if (byteCount < 0) {
		return false;
	}
	if (byteCount == 0) {
		return true;
	}

	std::vector<Packet*> packets;

	char* itr = buffer;
	for (int i = 0; i < byteCount; i++) {
		printf("%hhi, ", *(itr++));
	}
	puts("");

	InputMemoryBitStream readstream(buffer, byteCount * 8);
	printf("processing packets: ");
	while (readstream.GetRemainingBitCount() > 32) {
		unsigned int typeID;
		readstream.Read(typeID);
		printf("%u, ", typeID);

		if (typeID - 1 > makers.size()) {
			return true;
		}
		Packet* ptr = makers[typeID-1]();
		ptr->type = (PacketType)typeID;
		ptr->Read(readstream);
		packets.push_back(ptr);
	}
	puts("");

	for (Packet* packet : packets) {
		TCPPacketHandler handler = handlers[((int)packet->type)-1];
		if (handler) handler(packet);

		delete packet;
	}
	return true;
}


void PacketManager::ProcessPacket(Packet* packet) {

	TCPPacketHandler handler = handlers[((int)packet->type) - 1];
	if (handler) handler(packet);
	delete packet;
}

void PacketManager::QueuePacket(Packet* packet, uint32_t current_timestamp) {
	
	uint32_t startSize = packetStream.GetByteLength();
	unsigned int typeID = (unsigned int)packet->type;
	packetStream.Write(typeID);
	packet->Write(packetStream);

	uint32_t size = packetStream.GetByteLength() - startSize;

	packetQueue.push({ size, current_timestamp });
}

void PacketManager::SendQueuedPackets(UDPSocketPtr socket, const SocketAddress& addr, uint32_t current_timestamp) {

	uint32_t bytesToWrite = 0;
	uint32_t packetCount = 0;
	while (packetQueue.size() > 0) {
		QueuedPacket packet = packetQueue.front();
		if (packet.timestamp + 500> current_timestamp ) break;

		bytesToWrite += packet.size;
		packetQueue.pop();
		packetCount++;

		printf(" %u |", packet.size);
	}
	if (bytesToWrite == 0) return;

	printf("%u: ", packetCount);
	
	const char* itr = packetStream.GetBufferPtr();
	for (int i = 0; i < bytesToWrite; i++) {
		printf("%hhi, ", *(itr++));
	}
	puts("");
	int sent = socket->SendTo(packetStream.GetBufferPtr(), bytesToWrite, addr);
	if (sent != bytesToWrite) puts("FAILED TO WRITE ALL BYTES");
	packetStream.ShiftForward(bytesToWrite);
}