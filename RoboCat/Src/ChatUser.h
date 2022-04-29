#pragma once

#include "RoboCatPCH.h"


struct ChatUser
{
	OutputWindow win;
	std::string username;
	int sendRecvFlag = -1;
	bool quit;

	TCPSocketPtr sendSocket, recvSocket, recvConnSocket;

	ChatUser();
	~ChatUser();

	void initTcpClient(std::string sendPort, std::string recvPort);
	void initTcpServer(std::string listenPort);

	void send(std::string msg);
	void recv(std::string msg);

	void closeSockets();
	void shutdown();
};