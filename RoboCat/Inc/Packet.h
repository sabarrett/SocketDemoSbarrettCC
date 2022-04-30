#pragma once
#include <vector>
#include "MemoryBitStream.h"
#include <queue>

enum class PacketType {
	NONE,
	MOVE,
	DESTROY,
	CREATE,
	PLAYERINFO
};


class Packet {
public:
	PacketType type = PacketType::NONE;

	virtual ~Packet() = default;
	virtual void Read(InputMemoryBitStream& bitstream) = 0;
	virtual void Write(OutputMemoryBitStream& bitstream) = 0;
};


typedef Packet* (*TCPPacketMaker)();

// each packet type has a specilzation of this
template <typename T>
Packet* InternalTCPPacketMaker();


typedef void (*TCPPacketHandler)(Packet*);

class TCPSocket;


struct QueuedPacket {
	uint32_t size;
	uint64_t timestamp;
};


class PacketManager {
	std::vector<TCPPacketMaker> makers;
	std::vector<TCPPacketHandler> handlers;

	std::queue<QueuedPacket> packetQueue;
	OutputMemoryBitStream packetStream;
public:
	template <typename T>
	void RegisterType() {
		size_t i = (size_t)T::TYPE;
		if (makers.size() < i)
			makers.resize(i);
		makers[i-1] = InternalTCPPacketMaker<T>;
	}

	template <typename T>
	void RegisterHandler(void (*handler)(T*)) {
		size_t i = (size_t)T::TYPE;
		if (handlers.size() < i)
			handlers.resize(i);
		handlers[i-1] = (TCPPacketHandler)handler;
	}


	void SendQueuedPackets(UDPSocketPtr socket, const SocketAddress& addr, uint64_t timestamp);
	bool HandleInput(UDPSocketPtr socket, SocketAddress& out_addr);
	void ProcessPacket(Packet* packet);
	void QueuePacket(Packet* packet, uint64_t current_timestamp);


public:

};








class PacketDestroy : public Packet {
public:
	static const PacketType TYPE = PacketType::DESTROY;
	int X = 0, Y = 0;

	virtual void Read(InputMemoryBitStream& bitstream) override;
	virtual void Write(OutputMemoryBitStream& bitstream) override;
};

template <>
Packet* InternalTCPPacketMaker<PacketDestroy>();



class PacketMove : public Packet {
public:
	static const PacketType TYPE = PacketType::MOVE;

	int objectID = -1;
	uint64_t timestamp;
	Vector2 position = { 0, 0 };
	Vector2 velocity = { 0, 0 };


	virtual void Read(InputMemoryBitStream& bitstream) override;
	virtual void Write(OutputMemoryBitStream& bitstream) override;
};

template <>
Packet* InternalTCPPacketMaker<PacketMove>();



class PacketPlayerInfo : public Packet {
public:
	std::string nickname;


	static const PacketType TYPE = PacketType::PLAYERINFO;


	virtual void Read(InputMemoryBitStream& bitstream) override;
	virtual void Write(OutputMemoryBitStream& bitstream) override;
};



template <>
Packet* InternalTCPPacketMaker<PacketPlayerInfo>();