#pragma once

#include "GameObject.h"
#include "RoboCatPCH.h"

const float TIME_BETWEEN_PACKETS = 0.5f;

enum NetworkStates
{
	New,
	Waiting,
	Hello,
	Packet
};

class NetworkManager
{
	private:
		int mNetworkID; //here?
		float mLastHello;
		float mLastInputPacket;

		SocketAddress mAddress;
		TCPSocketPtr mSocket;
		NetworkStates mCurrentState;
	public:
		NetworkManager();
		~NetworkManager();

		void init(SocketAddress address);

		//maybe pass in a socket to use here, or have one in network manager
		void SendHello(); //OutputMemoryBitStream
		void SendPackets(); //OutputMemoryBitStream
		void ReceivePackets(InputMemoryBitStream& inputStream); //InputMemoryBitStream

		int getID();
};