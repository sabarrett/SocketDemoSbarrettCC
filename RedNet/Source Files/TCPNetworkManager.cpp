#include "TCPNetworkManager.h"

TCPNetworkManager* TCPNetworkManager::mspInstance = nullptr;

TCPNetworkManager::TCPNetworkManager()
{
	mSocket = nullptr;
	mConnection = nullptr;
}

TCPNetworkManager::~TCPNetworkManager()
{

}

TCPNetworkManager* TCPNetworkManager::getInstance()
{
	if (!mspInstance)
		mspInstance = new TCPNetworkManager();

	return mspInstance;
}

void TCPNetworkManager::cleanupInstance()
{
	if (mspInstance)
	{
		delete mspInstance;
		mspInstance = nullptr;
	}
}

void TCPNetworkManager::init(string address, int port, int reliabilityPercentage)
{

	SocketUtil::StaticInit();

	establishTCPSocket(address, port);

	srand(time(NULL));

	mReliabilityPercentage = reliabilityPercentage;
	
}

void TCPNetworkManager::cleanup()
{
	SocketUtil::CleanUp();
}

void TCPNetworkManager::establishTCPSocket(string address, int port)
{
	mSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (mSocket == nullptr)
	{
		SocketUtil::ReportError("Creating socket failed!");
		//ExitProcess(1);
	}

	string myAddress = address + ":" + std::to_string(port);

	SocketAddressPtr addressPtr = SocketAddressFactory::CreateIPv4FromString(myAddress);
	if (addressPtr == nullptr)
	{
		SocketUtil::ReportError("Creating address for socket failed!");
		//ExitProcess(1);
	}

	if (mSocket->Bind(*addressPtr) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding socket failed!");
		//ExitProcess(1);
	}
}

void TCPNetworkManager::connectTo(string address, int port)
{
	string connAddress = address + ":" + std::to_string(port);
	SocketAddressPtr addressPtr = SocketAddressFactory::CreateIPv4FromString(connAddress);
	if (addressPtr == nullptr)
	{
		SocketUtil::ReportError("Creating address for connection failed!");
		//ExitProcess(1);
	}

	if (mSocket->Connect(*addressPtr) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server failed!");
		//ExitProcess(1);
	}

	mConnection = mSocket;
}

void TCPNetworkManager::listenAndAccept()
{
	if (mSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening failed!");
		//ExitProcess(1);
	}

	SocketAddress connAddress;
	mConnection = mSocket->Accept(connAddress);

}

void TCPNetworkManager::sendMessage(string message)
{
	int r = rand() % 100;

	if (r < mReliabilityPercentage)
		mConnection->Send(message.c_str(), message.length());
	else
		std::cout << "Packet Dropped! Hehe" << std::endl;
}

void TCPNetworkManager::sendMessage(char* message, int length)
{
	int r = rand() % 100;

	if (r < mReliabilityPercentage)
		mConnection->Send(message, length);
	else
		std::cout << "Packet Dropped! Hehe" << std::endl;
}

void TCPNetworkManager::sendPacket(Packet_Header header, char* data, int length)
{
	mConnection->SetNonBlockingMode(false);

	int packetSize = sizeof(Packet_Header) + length;

	char buffer[4096];

	memcpy(buffer, (char*)&packetSize, sizeof(int));

	memcpy(buffer + sizeof(int), (char*)&header, sizeof(Packet_Header));

	memcpy(buffer + sizeof(int) + sizeof(Packet_Header), data, length);

	int r = rand() % 100;

	if (r < mReliabilityPercentage)
		sendMessage(buffer, sizeof(int) + sizeof(Packet_Header) + length);
	else
		std::cout << "Packet Dropped! Hehe" << std::endl;

}

string TCPNetworkManager::receiveMessage()
{
	char buffer[4096];
	int32_t bytesRead = mConnection->Receive(buffer, 4096);

	string msgReceived(buffer, bytesRead);
	return msgReceived;
}


void TCPNetworkManager::receivePackets(void(*handleFunction)(Packet_Header header, char* data, int length))
{
	mConnection->SetNonBlockingMode(true);

	char buffer[4096];
	char data[4096];
	int32_t bytesRead = mConnection->Receive(buffer, 4096);

	int bytesProcessed = 0;

	while (bytesProcessed < bytesRead)
	{
		int packetSize;

		memcpy(&packetSize, buffer + bytesProcessed, sizeof(int));
		bytesProcessed += sizeof(int);

		Packet_Header header;
		int32_t length = packetSize - sizeof(Packet_Header);

		memcpy(&header, buffer + bytesProcessed, sizeof(Packet_Header));
		bytesProcessed += sizeof(Packet_Header);

		memcpy(data, buffer + bytesProcessed, length);
		bytesProcessed += length;
		handleFunction(header, data, length);

		//std::cout << "Length: " << length << ", Packet Header: " << header << ", Data Length: " << length << std::endl;
	}

	

}
