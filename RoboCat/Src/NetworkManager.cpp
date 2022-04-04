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
	if (mCurrentState == NetworkStates::New && mLastHello > 0.5f)
	{
		//send out the hello packet thing
		mCurrentState = NetworkStates::Packet;
	}
}

void NetworkManager::SendPackets()
{
	if (mCurrentState == Packet)
	{

	}
}

void NetworkManager::ReceivePackets()
{

}

int NetworkManager::getID()
{
	return mNetworkID;
}
