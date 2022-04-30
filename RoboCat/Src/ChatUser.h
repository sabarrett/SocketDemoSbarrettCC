#pragma once

#include "RoboCatPCH.h"
#include <thread>


struct ChatUser
{
	OutputWindow win;
	std::string username;
	int sendRecvFlag = -1;
	bool quit;
	bool hasConnected = false;

	const std::string SEND_PORT = "7000", RECV_PORT = "8080";

	TCPSocketPtr sendSocket, recvSocket, recvConnSocket;
	std::thread t;

	ChatUser();
	~ChatUser();

	void startTcpThread(bool isClient);

	void initTcpClient();
	void initTcpServer();

	void send(std::string msg);
	void recv(std::string msg);

	void closeSockets();
	void shutdown();
};