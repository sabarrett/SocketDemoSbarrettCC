#include "NetworkManagerServer.h"

//put this in receiving packets, so receive and then chance to drop
/*
if( RoboMath::GetRandomFloat() >= mDropPacketChance )
			{
				//we made it
				//shove the packet into the queue and we'll handle it as soon as we should...
				//we'll pretend it wasn't received until simulated latency from now
				//this doesn't sim jitter, for that we would need to.....

				float simulatedReceivedTime = Timing::sInstance.GetTimef() + mSimulatedLatency;
				mPacketQueue.emplace( simulatedReceivedTime, inputStream, fromAddress );
			}
			else
			{
				LOG( "Dropped packet!", 0 );
				//dropped!
			}
*/

NetworkManagerServer::NetworkManagerServer()
{
	mName = "server";
	mLastPacket = 0.0f;
	mDropPacketChance = 10.0f;
	clientWelcomeNumber = 0;
	mSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	//mAddress = 
}

NetworkManagerServer::~NetworkManagerServer()
{
	
}

void NetworkManagerServer::init(SocketAddress address, std::string name)
{
	mServerAddress = address;
	mName = name;
	mLastPacket = 0.0f;
	mDropPacketChance = 10.0f;
	mSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	clientWelcomeNumber = 0;

	//bind socket?
	mSocket->Bind(address);
}

void NetworkManagerServer::update(GameObject objects[])
{
	mLastPacket += 0.00167;

	if (mLastPacket > TIME_BETWEEN_PACKETS)
	{
		mLastPacket = 0.0f;
		sendUpdatePacket(objects);
	}
}

void NetworkManagerServer::sendWelcomePacket()
{
	OutputMemoryBitStream output;
	
	std::string welcome = "welcome";

	output.Write(welcome);
	output.Write(clientWelcomeNumber);

	mSocket->Send(&output, sizeof(output));

	clientWelcomeNumber++;
}

void NetworkManagerServer::sendUpdatePacket(GameObject objects[])
{
	OutputMemoryBitStream output;

	output.Write(mObjectCount);
	
	for (int i = 0; i < mObjectCount; i++)
	{
		output.Write(mObjectTypes[i]);
		output.Write(mXPositions[i]);
		output.Write(mYPositions[i]);
	}

	mSocket->Send(&output, sizeof(output));
}

void NetworkManagerServer::receiveHelloPackets(InputMemoryBitStream& inputStream)
{
	mSocket->Receive(&inputStream, sizeof(inputStream));
	
	sendWelcomePacket();
}

void NetworkManagerServer::ReceiveUpdatePackets(InputMemoryBitStream& inputStream, SocketAddress fromAddress)
{
	//if (RoboMath::GetRandomFloat() >= mDropPacketChance)
	{
		//we made it
		//shove the packet into the queue and we'll handle it as soon as we should...
		//we'll pretend it wasn't received until simulated latency from now
		//this doesn't sim jitter, for that we would need to.....

		float simulatedReceivedTime = Timing::sInstance.GetTimef() + mSimulatedLatency;
		//mPacketQueue.emplace_back(simulatedReceivedTime, inputStream, fromAddress);
	}
	//else
	{
		LOG("Dropped packet!", 0);
		//dropped!
	}

	mSocket->Receive(&inputStream, sizeof(inputStream));

	int gameObjectCount;
	int posX;
	int posY;
	char objectType;

	inputStream.Read(gameObjectCount);

	mObjectCount = gameObjectCount;

	for (int i = 0; i < gameObjectCount; i++)
	{
		inputStream.Read(objectType);
		inputStream.Read(posX);
		inputStream.Read(posY);

		mObjectTypes[i] = objectType;
		mXPositions[i] = posX;
		mYPositions[i] = posY;
	}
}