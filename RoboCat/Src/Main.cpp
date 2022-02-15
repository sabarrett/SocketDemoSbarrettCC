
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>

//Global quit variable
bool gQuit = false;

void displayWelcomeMessage()
{
	std::cout << "\n\n\t\tWelcome to the chat room! Type your message and press Enter to send a message.\n\n";
}

//void displayDisconnectMessage(bool isMe, string userName = "")
//{
//	if (isMe)
//	{
//		std::cout << "You have disconnected from the chat room.\n";
//	}
//	else
//	{
//		std::cout << "User " << userName << " has disconnected from the chat room.\n";
//	}
//}

void setupTcpServer()
{
	//--------------------Setup Server--------------------
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

	//--------------------Chat Room--------------------
	
	//Welcome users
	displayWelcomeMessage();

	std::thread receiveThread([&]()	//TO-DO: COME BACK - don't use [&] :)
	{
		while (!gQuit) // Need to add a quit here to have it really exit!
		{
			char buffer[4096];
			int32_t bytesReceived = connSocket->Receive(buffer, 4096);
			if (bytesReceived == 0)
			{
				//TO-DO: handle disconnect
				gQuit = true;
				
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

	std::thread sendThread([&]()	//TO-DO: COME BACK - don't use [&] :)
	{
		while (!gQuit) // Need to add a quit here to have it really exit!
		{
			std::string msgToSend = "Oi there mate, 'bit rude to put my knoife in me chest, innit?";
			connSocket->Send(msgToSend.c_str(), msgToSend.length());
			std::cout << "Server sending message. \n";
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	});

	std::cout << "Press enter to exit at any time!\n";
	std::cin.get();
	gQuit = true;
	connSocket->~TCPSocket(); //TO-DO: Forcibly close socket (shouldn't call destructors like this -- make a new function for it!
	receiveThread.join();
	sendThread.join();
}

void setupTcpClient(std::string port)
{
	// Create socket
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
	}

	LOG("%s", "Client socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

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

	// Connect() -> Connect socket to remote host

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

	//Welcome users
	displayWelcomeMessage();

	//We bound the client socket to the client address, but we have the client socket connected to the server address

	////Client socket, connected to server address, is now listening on it -----------------------------------------WHY DO WE NOT NEED THIS?-------------------------------------------------------------
	//if (clientSocket->Listen() != NO_ERROR)
	//{
	//	SocketUtil::ReportError("Listening on listening socket");
	//	ExitProcess(1);
	//}

	//while (true)
	//{
		std::string msg("Hello server! How are you?");
		clientSocket->Send(msg.c_str(), msg.length());
		//std::this_thread::sleep_for(std::chrono::seconds(1));
	//}

	bool bQuit = false;
	std::thread receiveThreadClient([&]() { //TO-DO: COME BACK - don't use [&] :)
		std::cout << "receiveThreadClient running.\n";
		while (!bQuit) // Need to add a quit here to have it really exit!
		{
			char buffer[4096];
			int32_t bytesReceived = clientSocket->Receive(buffer, 4096);
			if (bytesReceived == 0)
			{
				//TO-DO: handle disconnect
				bQuit = true;
			}
			if (bytesReceived < 0)
			{
				SocketUtil::ReportError("Receiving");
				return;
			}

			std::string receivedMsg(buffer, bytesReceived);
			std::cout << "Received message from %s: %s" << servAddress->ToString() << receivedMsg << std::endl;
		}
	});

	std::cout << "Press enter to exit at any time!\n";
	std::cin.get();
	bQuit = true;
	clientSocket->~TCPSocket(); //TO-DO: Forcibly close socket (shouldn't call destructors like this -- make a new function for it!
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

	//OutputWindow win;

	//std::thread t([&win]()
	//			  {
	//				  int msgNo = 1;
	//				  while (true)
	//				  {
	//					  std::this_thread::sleep_for(std::chrono::milliseconds(250));
	//					  std::string msgIn("~~~auto message~~~");
	//					  std::stringstream ss(msgIn);
	//					  ss << msgNo;
	//					  win.Write(ss.str());
	//					  msgNo++;
	//				  }
	//			  });

	//while (true)
	//{
	//	std::string input;
	//	std::getline(std::cin, input);
	//	win.WriteFromStdin(input);
	//}

	bool isServer = StringUtils::GetCommandLineArg(1) == "server";

	//TO-DO: Custom usernames (add after welcome message and to disconnect message)
	//std::string userName;
	//std::getline(std::cin, userName);

	//Setup server and client
	if (isServer)
	{
		//Server code
		setupTcpServer();
	}
	else
	{
		//Client codes
		setupTcpClient(StringUtils::GetCommandLineArg(2));
	}

	SocketUtil::CleanUp();

	return 0;
}
