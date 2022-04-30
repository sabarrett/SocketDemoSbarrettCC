
#include "RoboCatPCH.h"




void PacketDestroy::Read(InputMemoryBitStream& bitstream) {
	bitstream.Read(X);
	bitstream.Read(Y);
}
void PacketDestroy::Write(OutputMemoryBitStream& bitstream) {
	bitstream.Write(X);
	bitstream.Write(Y);
}

template <>
Packet* InternalTCPPacketMaker<PacketDestroy>() {
	return (Packet*)new PacketDestroy();
}


void PacketMove::Read(InputMemoryBitStream& bitstream) {
	bitstream.Read(objectID);
	bitstream.Read(timestamp);
	bitstream.Read(position);
	bitstream.Read(velocity);
}

void PacketMove::Write(OutputMemoryBitStream& bitstream) {
	bitstream.Write(objectID);
	bitstream.Write(timestamp);
	bitstream.Write(position);
	bitstream.Write(velocity);
}

template <>
Packet* InternalTCPPacketMaker<PacketMove>() {
	return (Packet*)new PacketMove();
}

void PacketPlayerInfo::Read(InputMemoryBitStream& bitstream) {
	bitstream.Read(nickname);
}

void PacketPlayerInfo::Write(OutputMemoryBitStream& bitstream) {
	bitstream.Write(nickname);
}


template <>
Packet* InternalTCPPacketMaker<PacketPlayerInfo>() {
	return (Packet*)new PacketPlayerInfo();
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


	InputMemoryBitStream readstream(buffer, byteCount * 8);
	while (readstream.GetRemainingBitCount() > 32) {
		unsigned int typeID;
		readstream.Read(typeID);


		if (typeID - 1 > makers.size()) {
			return true;
		}
		Packet* ptr = makers[typeID-1]();
		ptr->type = (PacketType)typeID;
		ptr->Read(readstream);
		packets.push_back(ptr);
	}


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

void PacketManager::QueuePacket(Packet* packet, uint64_t current_timestamp) {
	
	uint32_t startSize = packetStream.GetByteLength();
	unsigned int typeID = (unsigned int)packet->type;
	packetStream.Write(typeID);
	packet->Write(packetStream);

	uint32_t size = packetStream.GetByteLength() - startSize;

	packetQueue.push({ size, current_timestamp });
}

void PacketManager::SendQueuedPackets(UDPSocketPtr socket, const SocketAddress& addr, uint64_t current_timestamp) {

	uint32_t bytesToWrite = 0;
	uint32_t packetCount = 0;
	while (packetQueue.size() > 0) {
		QueuedPacket packet = packetQueue.front();
		if (packet.timestamp + 250 > current_timestamp ) break; // fake lag

		bytesToWrite += packet.size;
		packetQueue.pop();
		packetCount++;
	}
	if (bytesToWrite == 0) return;

	int sent = socket->SendTo(packetStream.GetBufferPtr(), bytesToWrite, addr);
	if (sent != bytesToWrite) puts("FAILED TO WRITE ALL BYTES");
	packetStream.ShiftForward(bytesToWrite);
	puts("SENT PACKET");
}