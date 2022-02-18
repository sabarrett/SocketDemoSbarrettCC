#include "RoboCatPCH.h"
#include <iostream>
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

void DoTcpServer()
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

	bool quit = false;
	std::thread receiveThread([&]() { // don't use [&] :)
		while (!quit) // Need to add a quit here to have it really exit!
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
			std::string msg("Hello server! How are you?");
			connSocket->Send(msg.c_str(), msg.length());
		}
		});

	std::cout << "Press enter to exit at any time!\n";
	std::cin.get();
	quit = true;
	connSocket->~TCPSocket(); // Forcibly close socket (shouldn't call destructors like this -- make a new function for it!
	receiveThread.join();
}




void DoSocketRecieve(TCPSocketPtr socket, SocketAddressPtr address, bool& quit)
{
	while (!quit) // Need to add a quit here to have it really exit!
	{
		char buffer[4096];
		int32_t bytesReceived = socket->Receive(buffer, 4096);
		if (bytesReceived == 0)
		{
			// handle disconnect
			quit = true;
			std::cout << "User disconnected. Press enter to quit.\n";
			return;
		}
		if (bytesReceived < 0 && bytesReceived != -10035)
		{
			quit = true;
			SocketUtil::ReportError("Receiving");
			return;
		}
		else if (bytesReceived != -10035)
		{
			std::string receivedMsg(buffer, bytesReceived);
			LOG("Received message from %s: %s", address->ToString().c_str(), receivedMsg.c_str());
		}
		
	}
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

	LOG("Bound client socket at: %s", clientAddress->ToString().c_str());

	// Connect() -> Connect socket to remote host

	TCPSocketPtr connSocket;
	SocketAddressPtr connAddress;
	bool quit = false;
	bool isValid = false;
	std::string input;
	std::cout << "Ready to connect.\nType 'listen' to start listening for connections, 'connect' to connect to a listening client, or 'quit' to exit the program: ";
	
	while (!isValid)
	{
		std::cin >> input;

		
		if (input == "listen")
		{
			isValid = true;

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
			address = StringUtils::Sprintf("0.0.0.0:%s", port.c_str());
			SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
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
			clientSocket = listenSocket->Accept(incomingAddress);
			while (clientSocket == nullptr)
			{
				clientSocket = listenSocket->Accept(incomingAddress);
				// SocketUtil::ReportError("Accepting connection");
				// ExitProcess(1);
			}
			connAddress = SocketAddressFactory::CreateIPv4FromString(incomingAddress.ToString().c_str());
			LOG("Accepted connection from %s", incomingAddress.ToString().c_str());

		}
		else if (input == "connect")
		{
			isValid = true;
			//get user input for connectimg address and pass into function
			std::string inputA;
			std::cout << "Enter connection address: ";
			std::cin >> inputA;
			
			connAddress = SocketAddressFactory::CreateIPv4FromString(inputA.c_str());

			if (connAddress == nullptr)
			{
				SocketUtil::ReportError("Creating server address");
				ExitProcess(1);

			}
			if (clientSocket->Connect(*connAddress) != NO_ERROR)
			{
				SocketUtil::ReportError("Connecting to server");
				ExitProcess(1);
			}
			LOG("Connected to client at %s ", connAddress->ToString().c_str());


		}
		else if (input == "quit")
		{
			return;
		}
		else
		{
			std::cout << "Invalid input, please try again: ";
		}
	}
	clientSocket->SetNonBlockingMode(true);
	
	std::cout << "Type '/quit' at any time to disconnect and exit.\n";
	
	std::thread receiveThread(DoSocketRecieve, clientSocket, clientAddress, std::ref(quit));
	
	while (!quit)
	{
		std::string msg;

		std::getline(std::cin, msg);
		if (msg == "/quit")
		{
			quit = true;
			msg.clear();
			
		}
		else
			clientSocket->Send(msg.c_str(), msg.length());
	}

	
	receiveThread.join();
	
	return;
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
		//		Want P2P -- we'll get to that :)
		DoTcpServer();
	}
	else
	{
		// Client code ----------------
		DoTcpClient(StringUtils::GetCommandLineArg(2));
	}


	SocketUtil::CleanUp();

	return 0;
}