#include "RoboCatPCH.h"

#include <iostream>
#include <thread>

void receiveConnection(const TCPSocketPtr &sockPtr, int threadNum)
{
	SocketAddressPtr incomingAddress =
		SocketAddressFactory::CreateIPv4FromString("127.0.0.1");

	TCPSocketPtr connSocket = sockPtr->Accept(*incomingAddress);

	std::cout << "Thread " << threadNum << " connected" << std::endl;

	while (true)
	{
		char buffer[4096];
		int32_t bytesReceived = connSocket->Receive(buffer, 4096);

		if (bytesReceived == 0)
		{
			std::cout << "Connection on thread " << threadNum << " closed." << std::endl;
			break;
		}
		else if(bytesReceived < 0)
		{
			std::cout << "Connection on thread " << threadNum << " forcefully closed." << std::endl;
			break;
		}
		else
		{
			std::string receivedMsg(buffer, bytesReceived);
			std::cout << "\"" << receivedMsg.substr(0, receivedMsg.find("$")) << "\" says: " << receivedMsg.substr(receivedMsg.find("$") + 1) << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}
}

int __cdecl main(int argc, const char** argv)
{
	SocketUtil::StaticInit();

	TCPSocketPtr sockPtr = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

	std::string address = StringUtils::Sprintf("127.0.0.1:%s",
		StringUtils::GetCommandLineArg(1).c_str());
	SocketAddressPtr listenAddress =
		SocketAddressFactory::CreateIPv4FromString(address.c_str());

	if (sockPtr->Bind(*listenAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding listening socket");
		ExitProcess(1);
	}

	if (sockPtr->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on this socket");
		ExitProcess(1);
	}

	// Only 5 clients allowed in a chatroom
	std::thread receiveThread1(receiveConnection, std::ref(sockPtr), 1);
	std::thread receiveThread2(receiveConnection, std::ref(sockPtr), 2);
	std::thread receiveThread3(receiveConnection, std::ref(sockPtr), 3);
	std::thread receiveThread4(receiveConnection, std::ref(sockPtr), 4);
	std::thread receiveThread5(receiveConnection, std::ref(sockPtr), 5);

	TCPSocketPtr clientSock =
		SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

	address = StringUtils::Sprintf("127.0.0.1:%s",
		std::to_string(std::stoi(StringUtils::GetCommandLineArg(1)) + 1).c_str());
	clientSock->Bind(*SocketAddressFactory::CreateIPv4FromString(address.c_str()));

	std::string portToSend;
	std::cout << "Port to send to: " << std::endl;
	std::getline(std::cin, portToSend);

	std::string addressToSend = StringUtils::Sprintf("127.0.0.1:%s",
		portToSend.c_str());
	SocketAddressPtr sendAddress =
		SocketAddressFactory::CreateIPv4FromString(addressToSend.c_str());

	std::cout << "Address " << address << " is sending data to " << addressToSend << "." << std::endl;

	if (clientSock->Connect(*sendAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to different client");
		ExitProcess(1);
	}

	std::string username = address;

	while(true)
	{
		std::string msg;
		std::getline(std::cin, msg);

		// if msg starts with username 
		if (strncmp(msg.c_str(), "/username", strlen("/username")) == 0)
		{
			std::cout << "Username set to \"" << msg.substr(10) << "\"" << std::endl;
			username = msg.substr(10);
		}
		else if (msg == "/quit")
		{
			clientSock->~TCPSocket();
			break;
		}
		else
		{
			std::string msgWithUsername = username + "$" + msg;
			clientSock->Send(msgWithUsername.c_str(), msgWithUsername.length());
		}
	}

	receiveThread1.join();
	receiveThread2.join();
	receiveThread3.join();
	receiveThread4.join();
	receiveThread5.join();

	SocketUtil::CleanUp();
}