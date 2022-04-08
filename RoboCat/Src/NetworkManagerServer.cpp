#include "NetworkManagerServer.h"

NetworkManagerServer::NetworkManagerServer()
{
	mName = "server";
	mLastPacket = 0.0f;
}

NetworkManagerServer::~NetworkManagerServer()
{

}

void NetworkManagerServer::init(SocketAddress address, std::string name)
{
	mServerAddress = address;
	mName = name;
	mLastPacket = 0.0f;
}

void NetworkManagerServer::update(GameObject objects[])
{
	mLastPacket += 0.00167;

	if (mLastPacket > TIME_BETWEEN_PACKETS)
	{
		//SendUpdatePacket(objects);
	}
}
