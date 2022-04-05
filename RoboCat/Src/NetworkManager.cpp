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
}

void NetworkManager::update() //sdl_timer
{
	//mLastHello += timer. //trying to do something with sdl timer for this
	//mLastInputPacket +=
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

void NetworkManager::SendPackets()
{
	if (mCurrentState == Hello && mLastHello > TIME_BETWEEN_PACKETS)
	{
		SendHello();
	}

	if (mCurrentState == Packet) //&& mLastPacket > TIME_BETWEEN_PACKETS
	{
		OutputMemoryBitStream output;
		
		//i assume this is basically gathering all info for a state packet
		//output.Write(# of game objects);
		//output.Write(game object type); //for each object
		//output.Write(game object position); //for each game object

		//mSocket->Send(&output, sizeof(output));
	}
}

void NetworkManager::ReceivePackets(InputMemoryBitStream& inputStream)
{
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