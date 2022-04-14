#pragma once

#include "GameObject.h"
//#include "RoboCatPCH.h"
#include "NetworkManager.h"

//const float TIME_BETWEEN_PACKETS = 0.25f;

class NetworkManagerServer
{
	private:
		float mLastPacket;
		std::string mName;
		int clientWelcomeNumber;

		int objectCount;
		char objectTypes[100];
		int xPositions[100];
		int yPositions[100];

		SocketAddress mServerAddress;
		TCPSocketPtr mSocket;
	public:
		NetworkManagerServer();
		~NetworkManagerServer();

		void init(SocketAddress address, std::string name);
		void update(GameObject objects[]);

		void sendWelcomePacket();
		void sendUpdatePacket(GameObject objects[]);

		void receiveHelloPackets(InputMemoryBitStream& inputStream); //InputMemoryBitStream& inputStream
		void ReceiveUpdatePackets(InputMemoryBitStream& inputStream); //InputMemoryBitStream& inputStream
};