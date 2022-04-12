#pragma once
#include <vector>
#include "MemoryBitStream.h"

enum class PacketType {
	NONE,
	MOVE,
	DESTROY,
	CREATE,
	PLAYERINFO
};


class Packet {
public:
	PacketType type;

	virtual void Read(InputMemoryBitStream& bitstream) = 0;
	virtual void Write(OutputMemoryBitStream& bitstream) = 0;
};


typedef Packet* (*TCPPacketMaker)();

// each packet type has a specilzation of this
template <typename T>
static Packet* InternalTCPPacketMaker();


typedef void (*TCPPacketHandler)(Packet*);

class TCPSocket;



struct Packet_Destroy {
	int X, Y;
};




struct Packet_Move {

	int objectID;
	Vector2 position;
	Vector2 velocity;

};


struct Packet_PlayerInfo {
	std::string nickname;
};



class PacketManager {
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

	bool HandleInput(UDPSocketPtr socket, SocketAddress& out_addr);
	void ProcessPacket(Packet* packet);
	void SendPacket(UDPSocketPtr socket, const SocketAddress& addr, Packet* packet);


	void HandlePacket_Destroy(InputMemoryBitStream& bitstream);
	void HandlePacket_Move(InputMemoryBitStream& bitstream);
	void HandlePacket_PlayerInfo(InputMemoryBitStream& bitstream);


public:
	void SendPacket(Packet_Destroy pkt);
	void SendPacket(Packet_Move pkt);
	void SendPacket(Packet_PlayerInfo pkt);


private:
	void Send(TCPSocket* socket, OutputMemoryBitStream& stream);
};








class PacketDestroy : public Packet {
public:
	static const PacketType TYPE = PacketType::DESTROY;
	int X, Y;

	virtual void Read(InputMemoryBitStream& bitstream) override;
	virtual void Write(OutputMemoryBitStream& bitstream) override;
};

template <>
static Packet* InternalTCPPacketMaker<PacketDestroy>() {
	return (Packet*)new PacketDestroy();
}




class PacketMove : public Packet {
public:
	static const PacketType TYPE = PacketType::MOVE;

	int objectID;
	Vector2 position;
	Vector2 velocity;


	virtual void Read(InputMemoryBitStream& bitstream) override;
	virtual void Write(OutputMemoryBitStream& bitstream) override;
};

template <>
static Packet* InternalTCPPacketMaker<PacketMove>() {
	return (Packet*)new PacketMove();
}



class PacketPlayerInfo : public Packet {
public:
	std::string nickname;


	static const PacketType TYPE = PacketType::PLAYERINFO;


	virtual void Read(InputMemoryBitStream& bitstream) override;
	virtual void Write(OutputMemoryBitStream& bitstream) override;
};



template <>
static Packet* InternalTCPPacketMaker<PacketPlayerInfo>() {
	return (Packet*)new PacketPlayerInfo();
}