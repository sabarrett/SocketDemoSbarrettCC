#include "TCPNetworkManager.h"
#include <time.h>

TCPNetworkManager* TCPNetworkManager::mspInstance = nullptr;
int TCPNetworkManager::nextPacketID = 1;
int TCPNetworkManager::lastProcessedID = 0;

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

	delayedPackets = queue<Packet_Info>();
	ensuredPacketsWaiting = vector<Packet_Info>();

	resendTimer = 0.0f;
	
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
}

void TCPNetworkManager::sendMessage(char* message, int length)
{
	int r = rand() % 100;

	if (r < mReliabilityPercentage)
		mConnection->Send(message, length);
}

void TCPNetworkManager::sendPacket(Packet_Header header, char* data, int length, bool ensured, int overridePacketNum)
{
	mConnection->SetNonBlockingMode(false);

	int packetSize = sizeof(ensured) + sizeof(int) + sizeof(Packet_Header) + length;

	char buffer[4096];

	memcpy(buffer, (char*)&packetSize, sizeof(int));

	memcpy(buffer + sizeof(int), (char*)&header, sizeof(Packet_Header));

	memcpy(buffer + sizeof(int) + sizeof(Packet_Header), (char*)&ensured, sizeof(bool));

	if(overridePacketNum)
		memcpy(buffer + sizeof(int) + sizeof(Packet_Header) + sizeof(bool), &overridePacketNum, sizeof(int));
	else
		memcpy(buffer + sizeof(int) + sizeof(Packet_Header) + sizeof(bool), &nextPacketID, sizeof(int));

	memcpy(buffer + sizeof(int) + sizeof(int) + sizeof(Packet_Header) + sizeof(bool), data, length);

	if (ensured)
	{
		Packet_Info pck;
		pck.header = header;
		memcpy(pck.data, data, length);
		pck.length = length;
		pck.ensured = ensured; //always true because of inside if(ensured)

		if(overridePacketNum)
			pck.id = overridePacketNum;
		else
			pck.id = nextPacketID;

		bool match = false;

		for (auto i = ensuredPacketsWaiting.begin(); i != ensuredPacketsWaiting.end(); i++)
		{
			if (i->id == pck.id)
			{
				match = true;
				break;
			}
		}

		if(!match)
			ensuredPacketsWaiting.push_back(pck);
	}

	int r = rand() % 100;

	if (r < mReliabilityPercentage)
		sendMessage(buffer, sizeof(int) + sizeof(int) + sizeof(Packet_Header) + length + sizeof(bool));
	else
	{
		r = false;// rand() % 2;
		if(r)
		{
			Packet_Info temp;
			if (delayedPackets.size() >= 3)
			{
				temp = delayedPackets.front();
				delayedPackets.pop();
				sendPacket(temp.header, temp.data, temp.length, temp.ensured, temp.id);
			}

			temp.header = header;
			memcpy(temp.data, data, length);
			temp.length = length;
			temp.ensured = ensured;

			if (overridePacketNum)
				temp.id = overridePacketNum;
			else
				temp.id = nextPacketID;

			delayedPackets.emplace(temp);

			//std::cout << "Packet Stored! Hehe" << std::endl;
		}
	}

	if(!overridePacketNum)
		nextPacketID++;
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
		bool ensured;
		int id;

		memcpy(&packetSize, buffer + bytesProcessed, sizeof(int));
		bytesProcessed += sizeof(int);

		Packet_Header header;
		int32_t length = packetSize - sizeof(Packet_Header) - sizeof(bool) - sizeof(int);

		memcpy(&header, buffer + bytesProcessed, sizeof(Packet_Header));
		bytesProcessed += sizeof(Packet_Header);

		memcpy(&ensured, buffer + bytesProcessed, sizeof(bool));
		bytesProcessed += sizeof(bool);

		memcpy(&id, buffer + bytesProcessed, sizeof(int));
		bytesProcessed += sizeof(int);

		std::cout << "Received ID: " << id << std::endl;

		memcpy(data, buffer + bytesProcessed, length);
		bytesProcessed += length;

		if (ensured) //If this packet I received is ensured, send an acknoledgement.
		{
			sendPacket(ACKNOLEDGEMENT, data, length, false, id);
		}
		else
		{
			if (header == ACKNOLEDGEMENT)
			{
				//Avoids the continue below to process the ACKNOLEDGEMENT
			}
			else if (id > lastProcessedID)
			{
				lastProcessedID = id;
			}
			else
			{
				std::cout << "Network Manager Dropped Packet!!" << std::endl;
				continue;
			}
		}

		if (header == ACKNOLEDGEMENT) //If this packet is an acknoledgement, mark it as received.
		{
			for (auto i = ensuredPacketsWaiting.begin(); i != ensuredPacketsWaiting.end(); i++)
			{
				if (i->id == id)
				{
					ensuredPacketsWaiting.erase(i);
					break;
				}
			}
		}
		else
			handleFunction(header, data, length);

		//std::cout << "Length: " << length << ", Packet Header: " << header << ", Data Length: " << length << std::endl;
	}

}

void TCPNetworkManager::update(float deltaTime, float currentTime)
{

	if (ensuredPacketsWaiting.size() > 0)
	{
		if (resendTimer == 0.0f)
		{
			resendTimer = 1.0f;
			return;
		}
		else if (resendTimer > 0.0f)
		{
			resendTimer -= deltaTime;
		}
		else
		{
			int size = ensuredPacketsWaiting.size();
			for (int j = 0; j < size; j++)
			{
				Packet_Info i = ensuredPacketsWaiting.at(j);
				sendPacket(i.header, i.data, i.length, true, i.id);
			}

			resendTimer = 1.0f;
		}
	}
	else
		resendTimer = 0.0f;

}
