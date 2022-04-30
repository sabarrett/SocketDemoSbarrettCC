#include "NetworkManagerServer.h"

NetworkManagerServer::NetworkManagerServer()
{
	mName = "server";
	mLastPacket = 0.0f;
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

void NetworkManagerServer::ReceiveUpdatePackets(InputMemoryBitStream& inputStream)
{
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