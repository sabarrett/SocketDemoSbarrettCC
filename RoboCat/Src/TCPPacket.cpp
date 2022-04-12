
#include "RoboCatPCH.h"


void TCPPacketDestroy::Read(InputMemoryBitStream& bitstream) {
	bitstream.Read(X);
	bitstream.Read(Y);
}
void TCPPacketDestroy::Write(OutputMemoryBitStream& bitstream) {
	bitstream.Write(X);
	bitstream.Write(Y);
}

void TCPPacketMove::Read(InputMemoryBitStream& bitstream) {
	bitstream.Read(objectID);
	bitstream.Read(position);
	bitstream.Read(velocity);
}

void TCPPacketMove::Write(OutputMemoryBitStream& bitstream) {
	bitstream.Write(objectID);
	bitstream.Write(position);
	bitstream.Write(velocity);
}

void TCPPacketPlayerInfo::Read(InputMemoryBitStream& bitstream) {
	bitstream.Read(nickname);
}

void TCPPacketPlayerInfo::Write(OutputMemoryBitStream& bitstream) {
	bitstream.Write(nickname);
}

void TCPPacketCreate::Read(InputMemoryBitStream& bitstream) {
	bitstream.Read(objectID);
}
void TCPPacketCreate::Write(OutputMemoryBitStream& bitstream) {
	bitstream.Write(objectID);
}



bool TCPPacketManager::HandleInput(TCPSocket* socket) {
	if (!socket->HasRead()) return true;

	char buffer[1024];
	int byteCount = socket->Receive(buffer, 1024);
	if (byteCount < 0) {
		return false;
	}

	std::vector<TCPPacket*> packets;

	InputMemoryBitStream readstream(buffer, byteCount * 8);
	while (readstream.GetRemainingBitCount() > 7) {
		unsigned int typeID;
		readstream.Read(typeID);

		TCPPacket* ptr = makers[typeID-1]();
		ptr->type = (TCPPacketType)typeID;
		ptr->Read(readstream);
		packets.push_back(ptr);
	}

	for (TCPPacket* packet : packets) {
		TCPPacketHandler handler = handlers[((int)packet->type)-1];
		if (handler) handler(packet);

		delete packet;
	}
	return true;
}


void TCPPacketManager::SendPacket(TCPSocket* socket, TCPPacket* packet) {
	
	OutputMemoryBitStream outstream;
	unsigned int typeID = (unsigned int)packet->type;
	outstream.Write(typeID);
	packet->Write(outstream);
	socket->Send(outstream.GetBufferPtr(), outstream.GetByteLength());
}