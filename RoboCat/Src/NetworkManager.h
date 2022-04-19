#pragma once

#include "GameObject.h"
#include "RoboCatPCH.h"
#include <vector>

const float TIME_BETWEEN_PACKETS = 0.25f;

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

		int mGameObjectCount;
		char mObjectTypes[100];
		int mXPositions[100]; //maybe just have array parameter instead of this
		int mYPositions[100];

		SocketAddress mAddress;
		TCPSocketPtr mSocket;
		NetworkStates mCurrentState;
	public:
		NetworkManager();
		~NetworkManager();

		void init(SocketAddress address, std::string name);
		void update(GameObject objects[]);

		//maybe pass in a socket to use here, or have one in network manager
		void SendHello();
		void SendPackets(GameObject objects[]); //GameObject objects[]
		void ReceivePackets(InputMemoryBitStream& inputStream);

		int getID();
		string getName();
};