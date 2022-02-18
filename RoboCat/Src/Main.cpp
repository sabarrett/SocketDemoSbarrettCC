
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

std::atomic<bool> isInAConnection{ false };
std::atomic<bool> closingConnections { false };

/// <summary>
/// Based from the DoTcpServer() in the in-class demo
/// </summary>
void OldSetUpListening()
{
	// Open a TCP socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listen socket");
		ExitProcess(1);
	}

	LOG("%s", "Created listen socket");

	//SocketAddress a2(INADDR_LOOPBACK, 8080);
	// Listen only for connections on this machine
	SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:8080");   

	if (a == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	LOG("%s", "Create socket address");

	int startingAddress = 8080;

	if (listenSocket->Bind(*a) != NO_ERROR)
	{
		for (int i = 0; i < 20; i++)
		{
			++startingAddress;
			a = SocketAddressFactory::CreateIPv4FromString("000.0.0.0:" + std::to_string(startingAddress));
			if (listenSocket->Bind(*a) == NO_ERROR)
				break;
		}
	}

	// "Bind" that socket to the address we want to listen on
	if (listenSocket->Bind(*a) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding socket");
		ExitProcess(1);
	}
	

	LOG("%s", "Bound socket");

	// Call "Listen" to have the OS listen for connections on that socket
	if (listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening");
		ExitProcess(1);
	}

	LOG("%s", "Socket listening");

	// Call "Accept" which *blocks* until we get a request to connect,
	// and then it accepts that connection!


	/// listenSocket <----- packets from other hosts wanting to connect
	/// listenSocket -----> accepts connection, spawns:
	///   connectionSocket <----> otherHost1
	///   connectionSock2  <----> otherHost2

	SocketAddress connAddr;
	TCPSocketPtr conn;

	TCPSocketPtr connSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (connSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listen socket");
		ExitProcess(1);
	}

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	if (servAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	if (connSocket->Connect(*servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}


	while (conn == nullptr)
	{
		LOG("%s", "Waiting For Connection");
		conn = listenSocket->Accept(connAddr);
	}




	LOG("%s", "Connected to server!");

	std::string msg("Hello, server! How are you today?");
	connSocket->Send(msg.c_str(), msg.length());

	LOG("%s", "Sent message to server");


	// This code isn't blocking anymore -- it'll run to the end of the program.

	char buffer[4096];
	int32_t bytesRead = conn->Receive(buffer, 4096);

	std::string msgReceived(buffer, bytesRead);
	LOG("Received message: %s", msgReceived.c_str());
}

void SetUpListening(TCPSocketPtr listeningSocket, SocketAddressPtr listeningAddress)
{
	//  1
		// here we can select which IPV we're using, and IPV6 is a bit wild for these purposes so, we go with IPV4
	listeningSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listeningSocket == nullptr)
	{
		SocketUtil::ReportError("Error Creating Listen Socket");
		ExitProcess(1);
	}
	LOG("%s","created listening socket");

	listeningAddress = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:8080");
	if (listeningAddress == nullptr)
	{
		SocketUtil::ReportError("Error creating server address");
		ExitProcess(1);
	}

	int listeningAddressPort = 8080;

	LOG("%s", "binding the socket");
	while (listeningSocket->Bind(*listeningAddress) != NO_ERROR)
	{

		LOG("%s", "port: 0.0.0.0:" + std::to_string(listeningAddressPort) + " taken, trying to use port: 0.0.0.0:" + std::to_string(listeningAddressPort + 1));
		++listeningAddressPort;
		listeningAddress = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:" + std::to_string(listeningAddressPort));
	}

	LOG("%s", "bound the socket");

	if (listeningSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Error listening with the socket");
		ExitProcess(1);
	}


	LOG("Your port is %i", listeningAddressPort)
	LOG("%s", "socket is now listening");

					//  connection establishment
}

void HandleListening(TCPSocketPtr listeningSocket, SocketAddressPtr listeningAddress, vector<TCPSocketPtr> listeningConnectionSockets, queue<string> messages)
{

	//  4
	//  listenSocket     <====== packets from other hosts that want to connect
	//  listenSocket     ======> accepts connection, which creates:
	//  connectionSocket <=====> otherHost1
	//  connectionSock2  <=====> otherHost2

	SocketAddress connAddr;

	while (closingConnections)
	{
		for each (TCPSocketPtr conn in listeningConnectionSockets)
		{
			if (conn == nullptr)
			{
				LOG("%s", "socket is still listening");
				//  TODO - This may be a bit wasteful
				conn = listeningSocket->Accept(connAddr);
			}
			else
			{
				char buffer[4096];
				int32_t bytesRead = conn->Receive(buffer, 4096);   // make sure that you ALWAYS line up to the buffer sizes so people don't slip in a little extra

				std::string msgRecieved(buffer, bytesRead);
				LOG("Recieved message: %s", msgRecieved.c_str());
				messages.push(buffer);
			}

		}
		//conn->Receive();
	}
}

void SetUpSending(int portToConnectTo)
{
	//  1
// here we can select which IPV we're using, and IPV6 is a bit wild for these purposes so, we go with IPV4
	TCPSocketPtr connSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (connSocket == nullptr)
	{
		SocketUtil::ReportError("Error Creating Listen Socket");
		ExitProcess(1);
	}
	LOG("%s", "created connection socket");

	SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:8081");
	if (a == nullptr)
	{
		SocketUtil::ReportError("Error creating sending address");
		ExitProcess(1);
	}
	LOG("%s", "created connection socket address");

	int connectionAddressPort = 8081;
	LOG("%s", "binding the connection socket");
	while (connSocket->Bind(*a) != NO_ERROR)
	{
		LOG("%s", "port: 0.0.0.0:" + std::to_string(connectionAddressPort) + " taken, using port: 0.0.0.0:" + std::to_string(connectionAddressPort + 1));
		++connectionAddressPort;
		a = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:" + std::to_string(connectionAddressPort));
	}
	LOG("%s", "finished binding the connection socket");
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

	OutputWindow win;

	//std::thread t_Sending(SetUpSending);


	SocketAddressPtr listeningAddress;
	TCPSocketPtr listeningSocket;
	vector<TCPSocketPtr> listeningConnectionSockets;
	std::map<int, string> connectedMembers;

	queue<string> messages;


	//conn->Receive();

	int answer = -1;
	
	// the main loop
	do
	{
		// pre-chat menu, for connecting/creating rooms
		do
		{
			std::cout << "Would you like to:\n"
				<< " 1 - Make a session,\n"
				<< " 2 - Join a session, or\n"
				<< " 3 - Exit the program?\n";

			std::cin >> answer;
		} while (!(answer > 0 && answer < 4));

		switch (answer)
		{
		case 1:
			break;

		case 2:
			break;

		case 3:
		{
			std::cout << "\n\nHave a nice day! (Press enter to exit).\n\n";
			string temp;
			std::cin >> temp;
			exit(0);
			break;
		}

		default:
			std::cout << "\n\nERROR: INVALID MENU CHOICE GOT THROUGH.\n\n";
			exit(1);

		}
		SetUpListening(listeningSocket, listeningAddress);

		std::cout << "\n\n What is the port of the person you would like to connect to?\n";
		int portToConnectTo;
		std::cin >> portToConnectTo;

		std::thread t([&win]()
			{
				int msgNo = 1;
				while (true)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(250));
					std::string msgIn("~~~auto message~~~");
					std::stringstream ss(msgIn);
					ss << msgNo;
					win.Write(ss.str());
					msgNo++;
				}
			});

		while (true)
		{
			std::string input;
			std::getline(std::cin, input);
			win.WriteFromStdin(input);
		}

	} while (answer != 3);


	
	//t_Sending.join();
	//t_Listening.join();

	SocketUtil::CleanUp();

	return 0;
}
