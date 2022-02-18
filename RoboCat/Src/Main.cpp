#include "RoboCatPCH.h"
#include <iostream>
#include <istream>
#include <thread>
#include <mutex>

// Problem: Game Loop
//
// updateInput(); (make sure to not block here!)
//		conn->Receive(); !!! This blocks !!!
//			Two solutions:
//				- Non-Blocking Mode
//					On Receive(), check for -10035; that means "nothings wrong, but I didn't receive any data either"
// update();
// render();
// goto beginning;

void DoTcpServer(std::string port)
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

	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:8080");
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

	// Enter chat text
	std::cout << "\n\n\n---------------CHAT ROOM---------------\n\n\n" << std::endl;

	// Get username
	std::string username;

	std::cout << "Enter username: ";
	std::getline(std::cin, username);

	std::cout << "USERNAME SET TO: " << username << std::endl;

	std::cout << "\nCommands: \n /exit - quits program \n Pressing Enter Sends Message\n";
	std::cout << std::endl;

	// Send & Receive Messages
	bool quit = false;
	bool sendQuit = false;

	std::thread sendThreadServer([&]() { // don't use [&] :)
		std::cout << "Message: ";

		while (!sendQuit) // Need to add a quit here to have it really exit!
		{
			std::string msg;

			std::getline(std::cin, msg);

			if (msg == "/exit")
			{
				sendQuit = true;
				//connSocket->~TCPSocket();
				connSocket->Send(msg.c_str(), msg.length());
				std::cout << "User has disconnected.";
				ExitProcess(1);
				break;
			}
			else
			{
				msg = username + ": " + msg;
				connSocket->Send(msg.c_str(), msg.length());
				std::cout << std::endl;
				std::cout << "Message: ";
			}
		}
	});

	std::thread receiveThreadServer([&]() { // don't use [&] :)
		while (!quit) // Need to add a quit here to have it really exit!
		{
			char buffer[4096];
			int32_t bytesReceived = connSocket->Receive(buffer, 4096);
			if (bytesReceived == 0)
			{
				std::cout << "User has disconnected.";
				ExitProcess(1);
			}
			if (bytesReceived < 0)
			{
				SocketUtil::ReportError("Receiving");
				return;
			}

			std::string receivedMsg(buffer, bytesReceived);
			
			if (receivedMsg == "/exit")
			{
				std::cout << "User has disconnected.";
				ExitProcess(1);
			}
			else
			{
				std::cout << std::endl;
				std::cout << "\n[" << incomingAddress.ToString().c_str() << "] " << receivedMsg << std::endl;
				std::cout << std::endl;
				std::cout << "Message: ";
			}
		}
	});

	/*std::cout << "Press enter to exit at any time!\n";
	std::cin.get();
	quit = true;
	connSocket->~TCPSocket(); // Forcibly close socket (shouldn't call destructors like this -- make a new function for it!*/
	receiveThreadServer.join();
	sendThreadServer.join();
}

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

	// Enter chat text
	std::cout << "\n\n\n--------------- CHAT ROOM --------------- \n\n\n" << std::endl;

	// Get username
	std::string username;

	std::cout << "Enter username: ";
	std::getline(std::cin, username);

	std::cout << "USERNAME SET TO: " << username << std::endl;

	std::cout << "\nCommands: \n /exit - quits program \n Pressing Enter Sends Message\n";
	std::cout << std::endl;

	bool quit = false;
	bool sendQuit = false;

	std::thread sendThreadClient([&]() { // don't use [&] :)
		std::cout << "Message: ";

		while (!sendQuit) // Need to add a quit here to have it really exit!
		{
			std::string msg;
			std::getline(std::cin, msg);

			if (msg == "/exit")
			{
				sendQuit = true;
				//clientSocket->~TCPSocket();
				clientSocket->Send(msg.c_str(), msg.length());
				std::cout << "User has disconnected.";
				ExitProcess(1);
				break;
			}
			else
			{
				msg = username + ": " + msg;
				clientSocket->Send(msg.c_str(), msg.length());
				std::cout << std::endl;
				std::cout << "Message: ";
			}
		}
		});

	std::thread receiveThreadClient([&]() { // don't use [&] :)
		while (!quit) // Need to add a quit here to have it really exit!
		{
			char buffer[4096];
			int32_t bytesReceived = clientSocket->Receive(buffer, 4096);
			if (bytesReceived == 0)
			{
				std::cout << "User has disconnected.";
				ExitProcess(1);
			}
			if (bytesReceived < 0)
			{
				SocketUtil::ReportError("Receiving");
				return;
			}

			std::string receivedMsg(buffer, bytesReceived);

			if (receivedMsg == "/exit")
			{
				std::cout << "User has disconnected.";
				ExitProcess(1);
			}
			else
			{
				std::cout << std::endl;
				std::cout << "\n[" << servAddress->ToString() << "] " << receivedMsg << std::endl;
				std::cout << std::endl;
				std::cout << "Message: ";
			}
		}
	});

	receiveThreadClient.join();
	sendThreadClient.join();
}

std::mutex coutMutex;

void DoCout(std::string msg)
{
	for (int i = 0; i < 5; i++)
	{
		coutMutex.lock();  // can block!
		std::cout << msg << std::endl;
		coutMutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	std::cout << "Exiting loop gracefully\n";
}

bool gQuit;

void DoCoutLoop(std::string msg)
{
	while (!gQuit)
	{
		coutMutex.lock();  // can block!
		std::cout << msg << std::endl;
		coutMutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	std::cout << "Exiting loop gracfully\n";
}

void DoCoutLoopLocal(std::string msg, const bool& quit)
{
	while (!quit)
	{
		coutMutex.lock();  // can block!
		std::cout << msg << std::endl;
		coutMutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	std::cout << "Exiting loop gracfully\n";
}

void DoCoutAndExit(std::string msg)
{
	std::cout << msg << std::endl;

	std::cout << "Exiting 'loop' gracefully\n";
}

void DoThreadExample()
{
	// Thread Example

	// Ex. 1: Two cout's at once

	//DoCout();
	gQuit = false;
	bool quit = false;

	// Lambdas = anonymous functions = Functions with no name.
	//		max(5, 7) <- two 'anonymous' ints
	//			int five = 5, seven = 7; max(five, seven);
	//
	//	Lambda syntax: [](args) {body} <- a lambda!
	//		[] -> captures (can use variables from outside scope of function

	//  TcpSocketPtr s;
	//	std::thread receiveThread([&s]() {
	//			s->Receive(...);
	//		});
	//
	//  ReceiveOnSocket() {
	//		s->Receive		// Not global! What are we referencing here?
	//	}

	std::thread t1(DoCoutLoopLocal, "Hello from thread 1!", std::ref(quit));
	std::thread t2(DoCoutLoopLocal, "Thread 2 reporting in!", std::ref(quit));
	std::thread t3([&quit](std::string msg)
		{
			while (!quit)
			{
				std::cout << msg << std::endl;

				std::cout << "Exiting 'loop' gracefully\n";
			}
		}, "Thread 3 here!");

	std::cout << "Hello from the main thread!\n";

	std::cout << "Press enter to exit at any time.\n\n";
	std::cin.get();

	gQuit = true;
	quit = true;

	t1.join();
	t2.join();
	t3.join();
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

	// WinSock2.h
	//    https://docs.microsoft.com/en-us/windows/win32/api/winsock/


	SocketUtil::StaticInit();

	//DoThreadExample();


	bool isServer = StringUtils::GetCommandLineArg(1) == "server";

	if (isServer)
	{
		// Server code ----------------
		DoTcpServer(StringUtils::GetCommandLineArg(2));
	}
	else
	{
		// Client code ----------------
		DoTcpClient(StringUtils::GetCommandLineArg(2));
	}


	SocketUtil::CleanUp();

	return 0;
}