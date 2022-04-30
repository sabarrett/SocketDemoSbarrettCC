#pragma once
#include <RoboCatPCH.h>
#include "TCPSocket.h"
#include <thread>


class TCPServer
{
public:
	TCPSocketPtr listenSocket;
	TCPSocketPtr connSocket;
	SocketAddress incomingAddress;

	TCPServer::TCPServer()
	{
		listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
		if (listenSocket == nullptr)
		{
			SocketUtil::ReportError("Creating listening socket");
			LOG("%s", "Listening socket created");
			ExitProcess(1);
		}
	}

	void Bind(std::string IP)
	{
		SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString(IP);

		if (listenAddress == nullptr)
		{
			SocketUtil::ReportError("Creating listen address");

		}

		if (listenSocket->Bind(*listenAddress) != NO_ERROR)
		{
			SocketUtil::ReportError("Binding listening socket");
		}

		LOG("%s", "Bound listening socket");
	}

	void Listen()
	{
		if (listenSocket->Listen() != NO_ERROR)
		{
			SocketUtil::ReportError("Listening on listening socket");
			ExitProcess(1);
		}

		LOG("%s", "Listening on socket");
	}

	void Accept()
	{
		LOG("%s", "Waiting to accept connections...");
		SocketAddress incomingAddress;
		TCPSocketPtr connSocket = listenSocket->Accept(incomingAddress);
		while (connSocket == nullptr)
		{
			connSocket = listenSocket->Accept(incomingAddress);
			
		}

		LOG("Accepted connection from %s", incomingAddress.ToString().c_str());
	}
};

class TCPClient
{
public:
	TCPSocketPtr clientSocket;
	std::string port;
	std::string msg = "";

	TCPClient::TCPClient()
	{
		SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
		if (clientSocket == nullptr)
		{
			SocketUtil::ReportError("Creating client socket");
			ExitProcess(1);
		}

		LOG("%s", "Client socket created");

		port = 8080;
	}

	TCPClient::TCPClient(int p)
	{
		SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
		if (clientSocket == nullptr)
		{
			SocketUtil::ReportError("Creating client socket");
			ExitProcess(1);
		}

		LOG("%s", "Client socket created");

		port = p;
	}

	void Bind()
	{
		std::string address = StringUtils::Sprintf("127.0.0.1:%s", port.c_str());
		SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
		if (clientAddress == nullptr)
		{
			SocketUtil::ReportError("Creating client address");
			ExitProcess(1);
		}

		if (clientSocket->Bind(*clientAddress) != NO_ERROR)
		{
			SocketUtil::ReportError("Binding client socket");
			// This doesn't block!
			ExitProcess(1);
		}

		LOG("%s", "Bound client socket");

	}

	void Connect()
	{
		SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
		if (servAddress == nullptr)
		{
			SocketUtil::ReportError("Creating server address");
			ExitProcess(1);
		}

		if (clientSocket->Connect(*servAddress) != NO_ERROR)
		{
			SocketUtil::ReportError("Connecting to server");
			ExitProcess(1);
		}

		LOG("%s", "Connected to server!");

		while (true)
		{
			std::string msg("Hello server! How are you?");
			if (msg != "")
				clientSocket->Send(msg.c_str(), msg.length());
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	void Send()
	{
		clientSocket->Send(msg.c_str(), msg.length());
		msg = "";
	}
};