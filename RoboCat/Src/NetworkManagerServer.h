#pragma once

#include "GameObject.h"
//#include "RoboCatPCH.h"
#include "NetworkManager.h"
#include <vector>

//const float TIME_BETWEEN_PACKETS = 0.25f;

class NetworkManagerServer
{
	private:
		float mLastPacket;
		float mDropPacketChance;
		float mSimulatedLatency;
		std::string mName;
		int clientWelcomeNumber; //client network id

		int mObjectCount;
		char mObjectTypes[100];
		int mXPositions[100];
		int mYPositions[100];
		//std::string mClientNames[100]; //in case i need to track client names i am not sure

		SocketAddress mServerAddress;
		//SocketAddress mFromAddress;
		TCPSocketPtr mSocket;
		//vector<float, InputMemoryBitStream> mPacketQueue;
	public:
		NetworkManagerServer();
		~NetworkManagerServer();

		void init(SocketAddress address, std::string name);
		void update(GameObject objects[]);

		void sendWelcomePacket();
		void sendUpdatePacket(GameObject objects[]);

		void receiveHelloPackets(InputMemoryBitStream& inputStream); //InputMemoryBitStream& inputStream
		void ReceiveUpdatePackets(InputMemoryBitStream& inputStream, SocketAddress fromAddress); //InputMemoryBitStream& inputStream
};