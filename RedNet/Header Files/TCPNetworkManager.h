#pragma once
#include "..\..\RoboCat\Inc\RoboCatShared.h"
#include <string>
#include <queue>


enum Packet_Header
{
	INVALID_PACKET_TYPE = -1,
	PLAYER_MOVE,
	FIRE_PROJECTILE,
	OBJECT_MOVE,
	OBJECT_DELETE,
	CAMERA_MOVE,
	ACKNOLEDGEMENT,
	MAX_PACKET_TYPES
};

struct Packet_Info
{
	Packet_Header header;
	char data[4096];
	int length;
	int id;
	bool ensured;
};

class TCPNetworkManager
{

public:
	TCPNetworkManager();
	~TCPNetworkManager();

	void init(string address, int port, int reliabilityPercentage);
	void cleanup();

	static TCPNetworkManager* getInstance();
	static void cleanupInstance();

	void connectTo(std::string address, int port);
	void listenAndAccept();

	void update(float deltaTime, float currentTime);
	
	void sendPacket(Packet_Header, char* data, int length, bool ensured = false, int overridePacketNum = 0);
	
	void receivePackets(void (*handleFunction)(Packet_Header header, char* data, int length));

private:
	void establishTCPSocket(std::string address, int port);

	void sendMessage(char* data, int length);
	void sendMessage(string message);
	string receiveMessage();

	static TCPNetworkManager* mspInstance;

	static int nextPacketID;
	static int lastProcessedID;

	TCPSocketPtr mSocket;
	TCPSocketPtr mConnection;

	int mReliabilityPercentage;

	queue<Packet_Info> delayedPackets;

	vector<Packet_Info> ensuredPacketsWaiting;
	float resendTimer;
};
