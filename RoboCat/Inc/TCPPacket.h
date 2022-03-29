#pragma once
#include <vector>


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
static TCPPacket* InternalTCPPacketMaker();


typedef void (*TCPPacketHandler)(TCPPacket*);

class TCPSocket;

class TCPPacketManager {
	std::vector<TCPPacketMaker> makers;
	std::vector<TCPPacketHandler> handlers;

public:
	template <typename T>
	void RegisterType() {
		int i = (int)T::TYPE;
		if (makers.size() < i)
			makers.resize(i);
		makers[i-1] = InternalTCPPacketMaker<T>;
	}

	template <typename T>
	void RegisterHandler(void (*handler)(T*)) {
		int i = (int)T::TYPE;
		if (handlers.size() < i)
			handlers.resize(i);
		handlers[i-1] = (TCPPacketHandler)handler;
	}

	void HandleInput(TCPSocket* socket);
	void SendPacket(TCPSocket* socket, TCPPacket* packet);
};





class TCPPacketDestroy : public TCPPacket {
public:
	static const TCPPacketType TYPE = TCPPacketType::DESTROY;
	int objectID;

	virtual void Read(InputMemoryBitStream& bitstream) override;
	virtual void Write(OutputMemoryBitStream& bitstream) override;
};

template <>
static TCPPacket* InternalTCPPacketMaker<TCPPacketDestroy>() {
	return (TCPPacket*)new TCPPacketDestroy();
}




class TCPPacketMove : public TCPPacket {
public:
	static const TCPPacketType TYPE = TCPPacketType::MOVE;

	int objectID;
	Vector2 position;
	Vector2 velocity;


	virtual void Read(InputMemoryBitStream& bitstream) override;
	virtual void Write(OutputMemoryBitStream& bitstream) override;
};

template <>
static TCPPacket* InternalTCPPacketMaker<TCPPacketMove>() {
	return (TCPPacket*)new TCPPacketMove();
}


class TCPPacketCreate : public TCPPacket {
public:
	static const TCPPacketType TYPE = TCPPacketType::CREATE;

	int objectID;

	virtual void Read(InputMemoryBitStream& bitstream) override;
	virtual void Write(OutputMemoryBitStream& bitstream) override;
};

template <>
static TCPPacket* InternalTCPPacketMaker<TCPPacketCreate>() {
	return (TCPPacket*)new TCPPacketCreate();
}
