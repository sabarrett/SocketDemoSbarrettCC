
#include "RoboCatPCH.h"
#include <iostream>
#include <cstring>
#include <vector>
#include <thread>

void print(string str)
{
	std::cout << str << std::endl;
}

void Connect(string inputAddress, std::vector<TCPSocketPtr> &clientSockets)
{
	// Create socket
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
		return;
	}

	string address = inputAddress;
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError("Creating client address");
		ExitProcess(1);
		return;
	}

	if (clientSocket->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding client socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound client socket");

	// Connect() -> Connect socket to remote host

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
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
	else
	{
		clientSockets.push_back(clientSocket);
		print("Yay");
	}

}

void StartServer( bool &listenStatus)
{
	// Create socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}

	//listenSocket->SetNonBlockingMode(true);

	LOG("%s", "Listening socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	if (listenAddress == nullptr)
	{
		SocketUtil::ReportError("Creating listen address");
		ExitProcess(1);
	}

	if (listenSocket->Bind(*listenAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding listening socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound listening socket");

	// Blocking function call -> Waits for some input; halts the program until something "interesting" happens
	// Non-Blocking function call -> Returns right away, as soon as the action is completed

	// Listen() - Listen on socket -> Non-blocking; tells OS we care about incoming connections on this socket
	if (listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on listening socket");
		ExitProcess(1);
	}

	LOG("%s", "Listening on socket");

	// Accept() - Accept on socket -> Blocking; Waits for incoming connection and completes TCP handshake

	LOG("%s", "Waiting to accept connections...");
	SocketAddress incomingAddress;
	TCPSocketPtr connSocket = listenSocket->Accept(incomingAddress);
	while (connSocket == nullptr)
	{
		connSocket = listenSocket->Accept(incomingAddress);
		// SocketUtil::ReportError("Accepting connection");
		// ExitProcess(1);
	}

	LOG("Accepted connection from %s", incomingAddress.ToString().c_str());

	
	std::thread receiveThread([&]()
		{ // don't use [&] :)
			while (!listenStatus) // Need to add a quit here to have it really exit!
			{
				char buffer[4096];
				int32_t bytesReceived = connSocket->Receive(buffer, 4096);
				if (bytesReceived == 0)
				{
					// handle disconnect
				}
				if (bytesReceived < 0)
				{
					SocketUtil::ReportError("Receiving");
					return;
				}

				std::string receivedMsg(buffer, bytesReceived);
				LOG("Received message from %s: %s", incomingAddress.ToString().c_str(), receivedMsg.c_str());
			}
		});
}



#if _WIN32
int main(int argc, const char** argv)
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#else
const char** __argv;
int __argc;
int main(int argc, const char** argv)
{
	__argc = argc;
	__argv = argv;
#endif
	SocketUtil::StaticInit();
	string exit = "/exit";
	string help = "/help";
	string connect = "/connect";
	string listen = "/listen";


	bool running = true;
	bool listenStatus = false;
	std::vector<TCPSocketPtr> clientSockets;

	string input = "";
	while (running)
	{
		std::cin >> input;

		if (input[0] == '/')
		{

			if (input == exit)
			{
				running = false;
			}
			else if (input == help)
			{
				print("Helping...");
			}
			else if (input == connect)
			{
				string address;
				std::cin >> address;

				Connect(address, clientSockets);
			}
			else if (input == listen)
			{
				std::cin >> input;
				if (input == "off")
				{
					listenStatus = false;
				}
				else if(input == "on")
				{
					if (!listenStatus)
					{
						listenStatus = true;
						StartServer(listenStatus);
					}
				}
				print("Listening...");
			}
			else
			{
				print("Invalid command...");
			}

		}
		else
		{
			for (TCPSocketPtr socket : clientSockets)
			{
				socket->Send(input.c_str(), input.length());
			}
		}

		
	}


	

	SocketUtil::CleanUp();

	return 0;
}



