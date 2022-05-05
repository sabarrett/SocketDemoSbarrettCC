#pragma once

#include "RoboCatPCH.h"

struct PlayerUser
{
	int playerNumber;
	std::string playerName;
	std::string SEPERATOR_TOKEN = "!";
	int sendRecvFlag = -1;
	bool quit;
	float delayTimer = 0;


	TCPSocketPtr sendSocket, recvSocket, recvConnSocket;

	PlayerUser();
	PlayerUser(int _pnum);
	~PlayerUser();

	void initTcpClient(std::string sendPort, std::string recvPort);
	void initTcpServer(std::string listenPort);

	std::string messageIntoString(std::string _message);
	void decodeMessageString(std::string _messageString);
	void sendMessage(std::string _message);
	
	void closeSockets();
	void shutdown();

};