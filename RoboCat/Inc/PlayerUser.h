#pragma once
#include <thread>
#include "RoboCatPCH.h"

struct PlayerUser
{
	int playerNumber;
	std::string playerName;
	std::string SEPERATOR_TOKEN = "!";
	int sendRecvFlag = -1;
	bool quit;
	float delayTimer = 0;
	std::thread chatThread;

	std::string CLIENT_SEND_PORT = "1250", CLIENT_RECV_PORT = "2250";

	TCPSocketPtr sendSocket, recvSocket, recvConnSocket;

public:
	PlayerUser();
	PlayerUser(int _pnum, std::string username);
	~PlayerUser();

	std::string getUsername() { return playerName; };

	void startTcpThread(bool isServer);

	void initTcpClient(std::string sendPort, std::string recvPort);
	void initTcpServer(std::string listenPort);

	std::string messageIntoString(std::string _message);
	void decodeMessageString(std::string _messageString);
	void sendMessage(std::string _message);
	
	void closeSockets();
	void shutdown();

};