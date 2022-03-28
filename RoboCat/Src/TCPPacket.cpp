#include "TCPPacket.h"





void TCPPacketDestroy::Read(InputMemoryBitStream& bitstream) {
	bitstream.Read(objectID);
}
void TCPPacketDestroy::Write(OutputMemoryBitStream& bitstream) {
	bitstream.Write(objectID);
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