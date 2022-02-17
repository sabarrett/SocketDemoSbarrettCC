
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

void DoTcpServer();
void DoTcpClient(std::string port);
void ChatRoom(TCPSocketPtr socket, SocketAddress address);
void ChatRoom(TCPSocketPtr socket, SocketAddressPtr address);
bool CheckStringCaseInsensitive(string str1, string str2);

bool shouldQuit = false;

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
	if (StringUtils::GetCommandLineArg(1) == "server") //Server side
	{
		DoTcpServer();
	}
	else //Client side
	{
		DoTcpClient(StringUtils::GetCommandLineArg(2));
	}

	SocketUtil::CleanUp();
	return 0;
}

//Followed along from section 2 notes
void DoTcpServer()
{
	// Create socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}
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


	if (listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on listening socket");
		ExitProcess(1);
	}
	LOG("%s", "Listening on socket");



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

	ChatRoom(connSocket, incomingAddress);
}

//Followed along from section 2 notes
void DoTcpClient(std::string port)
{
	// Create socket
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
	}
	LOG("%s", "Client socket created");

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

	ChatRoom(clientSocket, servAddress);
}

void ChatRoom(TCPSocketPtr socket, SocketAddressPtr address)
{
	//Introduction Message
	std::cout << "Hello, you are successfully connected to the chat! Say hi! :).\n";
	std::cout << "Type '/quit' to quit the program\n\n";

	//Send a message
	std::thread sendThread([&]()
		{
			while (!shouldQuit)
			{
				std::string message;


				std::cin >> message;
				if (CheckStringCaseInsensitive(message, "/quit"))
				{
					shouldQuit = true;
					socket->~TCPSocket();
					break;
				}

				//Send via the socket
				socket->Send(message.c_str(), message.length());
			}
		});


	//Receive a message
	std::thread receiveThread([&]()
		{
			while (!shouldQuit)
			{
				//Retreive Message
				char buffer[4096];
				int32_t response = socket->Receive(buffer, 4096);

				//Error check
				if (response < 0)
				{
					SocketUtil::ReportError("Receiving Message");
					return;
				}

				//Unpack and display message
				std::string receivedMsg(buffer, response);
				std::cout << address->ToString() << ": " << receivedMsg << std::endl;
			}
		});

	//Handle Threads
	sendThread.join();
	receiveThread.join();
}

void ChatRoom(TCPSocketPtr socket, SocketAddress address)
{
	//Introduction Message
	std::cout << "Hello, you are successfully connected to the chat! Say hi! :).\n";
	std::cout << "Type '/quit' to quit the program\n\n";

	//Send a message
	std::thread sendThread([&]()
	{
		while(!shouldQuit)
		{
			std::string message;


			std::cin >> message;
			if (CheckStringCaseInsensitive(message, "/quit"))
			{
				shouldQuit = true;
				socket->~TCPSocket();
				break;
			}

			//Send via the socket
			socket->Send(message.c_str(), message.length());
		}
	});


	//Receive a message
	std::thread receiveThread([&]()
	{
		while (!shouldQuit)
		{
			//Retreive Message
			char buffer[4096];
			int32_t response = socket->Receive(buffer, 4096);

			//Error check
			if (response < 0)
			{
				SocketUtil::ReportError("Receiving Message");
				return;
			}

			//Unpack and display message
			std::string receivedMsg(buffer, response);
			std::cout << address.ToString() << ": " << receivedMsg << std::endl;
		}
	});

	//Handle Threads
	sendThread.join();
	receiveThread.join();
}

bool CheckStringCaseInsensitive(string str1, string str2)
{
	//convert s1 and s2 into lower case strings
	std::transform(str1.begin(), str1.end(), str1.begin(), ::tolower);
	std::transform(str2.begin(), str2.end(), str2.begin(), ::tolower);

	return str1.compare(str2) == 0 ? true : false;
}
