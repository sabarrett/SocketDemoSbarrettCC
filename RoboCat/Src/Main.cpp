
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>
#include <istream>

//Global quit variable
bool gQuit = false;

//The other user's username --------------------------- TO-DO: MAKE THIS A LIST IF YOU WANT MULTIPLE USERS
std::pair<SocketAddress, std::string> otherUsername;

void displayWelcomeMessage()
{
	std::cout << "\n\n\n\n\n\n\n\n\n\n\n\t\tWelcome to the chat room! Type your message and press Enter to send a message.\n\t\t\t Type '/exit' at any time to leave the room.\n\n\n\n\n";
}

std::string requestMyUsername()
{
	//Prompt for username
	std::string myUsername;
	std::cout << "\n\n\nEnter your username, then press Enter: ";
	std::getline(std::cin, myUsername);

	std::cout << std::endl << std::endl;

	return myUsername;
}

void setupTcpServer(std::string port)
{
	//--------------------Setup Server--------------------
	// Create socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}

	LOG("%s", "Listening socket created");

	//Ensure that your listening code can receive connections from any computer (hence the 0.0.0.0)
	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:0"/* + port*/);	//Have the client enter the EXACT SAME port number as the server
	if (listenAddress == nullptr)
	{
		SocketUtil::ReportError("Creating listen address");
		ExitProcess(1);
	}

	// Bind() - "Bind" socket -> tells OS we want to use a specific address
	if (listenSocket->Bind(*listenAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding listening socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound listening socket");

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

	{
		//Send username
		std:: string myName = requestMyUsername();
		connSocket->Send(myName.c_str(), myName.length());

		//Save received username
		char buffer[4096];
		int32_t bytesReceived = connSocket->Receive(buffer, 4096);

		//Error check
		if (bytesReceived <= 0)
		{
			SocketUtil::ReportError("Receiving");
			return;
		}

		//Save received username and socket address
		std::string receivedMsg(buffer, bytesReceived);
		otherUsername.first = incomingAddress;
		otherUsername.second = receivedMsg;
	}

	//Display who you connected to
	std::cout << "Connected to chat room with " << otherUsername.second << "!\n\n\n";

	//--------------------Chat Room--------------------
	
	//Welcome users
	displayWelcomeMessage();

	//Send
	std::thread sendThread([&connSocket]()
	{
		bool bClarifiedSender = false;

		while (!gQuit)
		{
			//Formatting
			if (!bClarifiedSender)
			{
				std::cout << "You: ";
				bClarifiedSender = true;
			}

			//Get input
			std::string input;
			std::getline(std::cin, input);

			//TO-DO: Send first byte as bool for user disconnect


			//Special case - exit message
			if (input == "/exit")
			{
				//Display disconnect message
				std::cout << "\n----------You have disconnected from the chat room with " << otherUsername.second << ".----------" << std::endl << std::endl;

				//Send newline message so it stops the getline
				string stop = " ";
				connSocket->Send(stop.c_str(), stop.length());

				//Cleanup and break out of the loop (basically end thread because loop condition will no longer be met)
				gQuit = true;
				connSocket->CleanupSocket();
				connSocket = nullptr;

				//TO-DO: Terminate other thread so it doesn't write that the other person disconnected


				break;
			}

			//Send message through the socket
			connSocket->Send(input.c_str(), input.length());
			std::cout << std::endl;
			bClarifiedSender = false;	//Reset
		}

		//Terminate this thread so the program ends - TO-DO: STOP EXCEPTIONS (figure out a different way)
		//std::terminate();
		return;
	});

	//Receive - I know we don't need both to be new threads, but it looks cleaner in my mind like this
	std::thread receiveThread([&connSocket]()
	{
		while (!gQuit)
		{
			//Buffer to receive strings
			char buffer[4096];
			int32_t bytesReceived = connSocket->Receive(buffer, 4096);

			//TO-DO:
			//Send first byte as bool for user disconnect
			//Check first byte with an array access [0]
			//Exit if it is false
			//Read message as remaining (cast to pointer and add 1 to the buffer so it reads starting at the first address)
			//Look into reinterpreting casts
			
			//Check for connection loss
			if (bytesReceived <= 0)
			{
				//Display disconnect message
				std::cout << "\n----------" << otherUsername.second << " has disconnected from the chat room.--------------------" << std::endl << std::endl;
				
				//Cleanup and break out of the loop (basically end thread because loop condition will no longer be met)
				gQuit = true;
				connSocket->CleanupSocket();
				connSocket = nullptr;
				break;
			}

			//Unpack and display message
			std::string receivedMsg(buffer, bytesReceived);
			std::cout << "Received message from " << otherUsername.second << ": " << receivedMsg << std::endl << std::endl;
		}

		//Terminate this thread so the program ends - TO-DO: STOP EXCEPTIONS (figure out a different way)
		//std::terminate();
		std::cout << "I'm still standing, yeah, yeah, yeah.\n";
		return;
	});

	//Join threads
	sendThread.join();
	receiveThread.join();
}

void setupTcpClient(std::string clientIpAddress, std::string serverIpAddress, std::string port)
{
	// Create socket
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
	}

	LOG("%s", "Client socket created");

	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(clientIpAddress + ":0");
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError("Creating client address");
		ExitProcess(1);
	}

	// Bind() - "Bind" socket -> tells OS we want to use a specific address
	if (clientSocket->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding client socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound client socket");

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString(serverIpAddress + ":" + port);	//Use IP address and port passed into the function ip:port
	if (servAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	// Connect() -> Connect socket to remote host
	if (clientSocket->Connect(*servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}

	{
		//Send username
		std::string myName = requestMyUsername();
		clientSocket->Send(myName.c_str(), myName.length());

		//Save received username
		char buffer[4096];
		int32_t bytesReceived = clientSocket->Receive(buffer, 4096);

		//Error check
		if (bytesReceived <= 0)
		{
			SocketUtil::ReportError("Receiving");
			return;
		}

		//Save received username and socket address
		std::string receivedMsg(buffer, bytesReceived);
		otherUsername.first = *servAddress;
		otherUsername.second = receivedMsg;
	}

	//Display who you connected to
	std::cout << "Connected to chat room with " << otherUsername.second << "!\n\n\n";

	//We bound the client socket to the client address, but we have the client socket connected to the server address

	//--------------------Chat Room--------------------

	//Welcome users
	displayWelcomeMessage();

	//Send
	std::thread sendThread([&clientSocket]()
	{
		bool bClarifiedSender = false;

		while (!gQuit)
		{
			//Formatting
			if (!bClarifiedSender)
			{
				std::cout << "You: ";
				bClarifiedSender = true;
			}

			//Get input
			std::string input;
			std::getline(std::cin, input);

			//Special case - exit message
			if (input == "/exit")
			{
				//Display disconnect message
				std::cout << "\n----------You have disconnected from the chat room with " << otherUsername.second << ".----------" << std::endl << std::endl;
				
				//Send newline message so it stops the getline
				string stop = " ";
				clientSocket->Send(stop.c_str(), stop.length());

				//Cleanup and break out of the loop (basically end thread because loop condition will no longer be met)
				gQuit = true;
				clientSocket->CleanupSocket();
				clientSocket = nullptr;

				//TO-DO: Terminate other thread so it doesn't write that the other person disconnected


				break;
			}

			//Send message through the socket
			clientSocket->Send(input.c_str(), input.length());
			std::cout << std::endl;
			bClarifiedSender = false;	//Reset
		}

		//Terminate this thread so the program ends - TO-DO: STOP EXCEPTIONS (figure out a different way)
		//std::terminate();
		return;
	});

	//Receive - I know we don't need both to be new threads, but it looks cleaner in my mind like this
	std::thread receiveThreadClient([&clientSocket]()
	{
		while (!gQuit)
		{
			//Buffer to receive strings
			char buffer[4096];
			int32_t bytesReceived = clientSocket->Receive(buffer, 4096);
			
			//Check for connection loss
			if (bytesReceived <= 0)
			{
				//Display disconnect message
				std::cout << "\n----------" << otherUsername.second << " has disconnected from the chat room.----------" << std::endl << std::endl;

				//Cleanup and break out of the loop (basically end thread because loop condition will no longer be met)
				gQuit = true;
				clientSocket->CleanupSocket();
				clientSocket = nullptr;
				break;
			}

			//Unpack and display message
			std::string receivedMsg(buffer, bytesReceived);
			std::cout << "Received message from " << otherUsername.second << ": " << receivedMsg << std::endl << std::endl;
		}

		//Terminate this thread so the program ends - TO-DO: STOP EXCEPTIONS (figure out a different way)
		//std::terminate();
		return;
	});

	//Join threads
	sendThread.join();
	receiveThreadClient.join();
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

	//0-indexed
	bool isServer = StringUtils::GetCommandLineArg(1) == "server";

	//Setup server and client
	if (isServer)
	{
		//Server code
		setupTcpServer(StringUtils::GetCommandLineArg(2));	//CommandLineArg(2) for port
	}
	else
	{
		//Client code
		setupTcpClient(StringUtils::GetCommandLineArg(2), StringUtils::GetCommandLineArg(3), StringUtils::GetCommandLineArg(4));	//CommandLineArg(2) for client IP address, CommandLineArg(3) for server IP address, CommandLineArg(4) for port
	}

	SocketUtil::CleanUp();

	return 0;
}
