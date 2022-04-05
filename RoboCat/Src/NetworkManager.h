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
		int mNetworkID; 
		float mLastHello;
		float mLastInputPacket;
		std::string mName;

		//int mGameObjectCount;

		SocketAddress mAddress;
		TCPSocketPtr mSocket;
		NetworkStates mCurrentState;
	public:
		NetworkManager();
		~NetworkManager();

		void init(SocketAddress address, std::string name);
		void update();

		//maybe pass in a socket to use here, or have one in network manager
		void SendHello();
		void SendPackets();
		void ReceivePackets(InputMemoryBitStream& inputStream);

		int getID();
		string getName();
};