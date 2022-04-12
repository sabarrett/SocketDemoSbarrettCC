
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


	char buffer[1024];
	int byteCount = socket->ReceiveFrom(buffer, 1024, out_addr);
	if (byteCount < 0) {
		return false;
	}
	if (byteCount == 0) {
		return true;
	}

	std::vector<Packet*> packets;

	InputMemoryBitStream readstream(buffer, byteCount * 8);
	while (readstream.GetRemainingBitCount() > 7) {
		unsigned int typeID;
		readstream.Read(typeID);

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

void PacketManager::SendPacket(UDPSocketPtr socket, const SocketAddress& addr, Packet* packet) {
	
	OutputMemoryBitStream outstream;
	unsigned int typeID = (unsigned int)packet->type;
	outstream.Write(typeID);
	packet->Write(outstream);
	socket->SendTo(outstream.GetBufferPtr(), outstream.GetByteLength(), addr);
}