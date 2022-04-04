#include "NetworkManager.h"

NetworkManager::NetworkManager()
{
	mNetworkID = 0;
	//mAddress = 
	mSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	mLastHello = 0.0f;
	mLastInputPacket = 0.0f;
	mCurrentState = NetworkStates::New;
}

NetworkManager::~NetworkManager()
{

}

void NetworkManager::init(SocketAddress address)
{
	mAddress = address;
	mSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	mLastHello = 0.0f;
	mLastInputPacket = 0.0f;
	mCurrentState = NetworkStates::New;
}

void NetworkManager::SendHello()
{
	if (mCurrentState == NetworkStates::New && mLastHello >= 0.5f)
	{
		//send out the hello packet thing
		//OutputMemoryBitStream
		mCurrentState = NetworkStates::Packet;
	}
}

void NetworkManager::SendPackets()
{
	if (mCurrentState == Hello)
	{
		SendHello();
	}

	if (mCurrentState == Packet)
	{
		OutputMemoryBitStream output;
		//output.Write(); //write the state packet out?
	}
}

void NetworkManager::ReceivePackets(InputMemoryBitStream& inputStream)
{

}

int NetworkManager::getID()
{
	return mNetworkID;
}
