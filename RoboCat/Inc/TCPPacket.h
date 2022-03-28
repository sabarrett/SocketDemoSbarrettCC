#pragma once
#include <raylib.h>
#include <vector>
#include "MemoryBitStream.h"


enum class TCPPacketType {
	NONE,
	MOVE,
	DESTROY,
	CREATE
};


class TCPPacket {
public:
	TCPPacketType type;

	virtual void Read(InputMemoryBitStream& bitstream) = 0;
	virtual void Write(OutputMemoryBitStream& bitstream) = 0;
};


typedef TCPPacket* (*TCPPacketMaker)();

// each packet type has a specilzation of this
template <typename T>
TCPPacket* InternalTCPPacketMaker();


typedef void (*TCPPacketHandler)(TCPPacket*);


class TCPPacketHandler {
	std::vector<TCPPacketMaker> makers;
	std::vector<TCPPacketHandler> handlers;
	
	template <typename T>
	void RegisterType() {
		makers[i] = InternalTCPPacketMaker<T>;
	}

	template <typename T>
	void RegisterHandler(void (handler)(T*)) {
		handlers[T::TYPE] = handler;
	}

	void NextPacket(int socket);
	void SendPacket(int socket, TCPPacket* packet);
};





class TCPPacketDestroy : public TCPPacket {
	static const TCPPacketType TYPE = TCPPacketType::DESTROY;
	int objectID;

	virtual void Read(InputMemoryBitStream& bitstream) override;
	virtual void Write(OutputMemoryBitStream& bitstream) override;
};

template <>
TCPPacket* InternalTCPPacketMaker<TCPPacketDestroy>() {
	return (TCPPacket*)new TCPPacketDestroy();
}




struct TCPPacketMove : public TCPPacket {
	static const TCPPacketType TYPE = TCPPacketType::MOVE;

	int objectID;
	Vector2 position;
	Vector2 velocity;


	virtual void Read(InputMemoryBitStream& bitstream) override;
	virtual void Write(OutputMemoryBitStream& bitstream) override;
};

template <>
TCPPacket* InternalTCPPacketMaker<TCPPacketDestroy>() {
	return (TCPPacket*)new TCPPacketMove();
}