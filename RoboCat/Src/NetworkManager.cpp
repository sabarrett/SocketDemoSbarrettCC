#include "NetworkManager.h"

NetworkManager::NetworkManager()
{
	mNetworkID = 0;
	//mAddress = 
	mSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	mLastHello = 0.0f;
	mLastInputPacket = 0.0f;
	mCurrentState = NetworkStates::New;
	mName = "client";
}

NetworkManager::~NetworkManager()
{

}

void NetworkManager::init(SocketAddress address, std::string name)
{
	mAddress = address;
	mSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	mLastHello = 0.0f;
	mLastInputPacket = 0.0f;
	mCurrentState = NetworkStates::New;
	mName = name;

	mSocket->Bind(address);
}

void NetworkManager::update(GameObject objects[])
{
	mLastHello += 0.00167; //trying to do something with sdl timer for this
	mLastInputPacket += 0.00167;

	if (mLastInputPacket > TIME_BETWEEN_PACKETS)
	{
		SendPackets(objects);
	}
}

void NetworkManager::SendHello()
{
	if (mCurrentState == NetworkStates::New && mLastHello >= 0.5f)
	{
		//send out the hello packet
		OutputMemoryBitStream sendHello;

		//sendHello.Write();
		sendHello.Write(mName);

		//SendPacket(sendHello);

		mSocket->Send(&sendHello, sizeof(sendHello)); //this might be totally wrong?
	}
}

void NetworkManager::SendPackets(GameObject objects[])
{
	if (mCurrentState == Hello && mLastHello > TIME_BETWEEN_PACKETS)
	{
		SendHello();
	}

	if (mCurrentState == Packet)
	{
		OutputMemoryBitStream output;
		
		//i assume this is basically gathering all info for a state packet
		output.Write(mGameObjectCount);

		for (int i = 0; i < mGameObjectCount; i++)
		{
			output.Write(mObjectTypes[i]);
			output.Write(mXPositions[i]);
			output.Write(mYPositions[i]);
		}

		mSocket->Send(&output, sizeof(output));
	}
}

void NetworkManager::ReceivePackets(InputMemoryBitStream& inputStream)
{
	mSocket->Receive(&inputStream, sizeof(inputStream));

	if (mCurrentState == NetworkStates::Hello)
	{
		int tmpID;
		inputStream.Read(tmpID);

		mNetworkID = tmpID;
		mCurrentState = NetworkStates::Packet;
	}
}

int NetworkManager::getID()
{
	return mNetworkID;
}

string NetworkManager::getName()
{
	return mName;
}