#include "RoboCatPCH.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <map>

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

const std::string EXIT_CODE = "!exit";

void DoTcpServer()
{
	std::map<SocketAddressPtr, std::string> usernameMap;
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
	//std::cout << listenAddress->ToString().c_str() << std::endl;
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

	std::string username;
	std::cout << "Please input your user name: ";
	getline(std::cin, username);
	//usernameMap.insert(std::pair<SocketAddressPtr, std::string>(listenAddress, username));
	usernameMap[listenAddress] = username;
	//std::cout << usernameMap[listenAddress] << std::endl;

	// Accept() - Accept on socket -> Blocking; Waits for incoming connection and completes TCP handshake
	SocketAddress incomingAddress;
	//TCPSocketPtr connSocket = listenForConnection(incomingAddress, listenSocket);
	LOG("%s", "Waiting to accept connections...");
	TCPSocketPtr connSocket = listenSocket->Accept(incomingAddress);
	while (connSocket == nullptr)
	{
		connSocket = listenSocket->Accept(incomingAddress);
		// SocketUtil::ReportError("Accepting connection");
		// ExitProcess(1);
	}

	connSocket->Send(username.c_str(), username.length());

	char buffer[4096];
	int32_t bytesReceived = connSocket->Receive(buffer, 4096);
	std::string receivedMsg(buffer, bytesReceived);
	SocketAddressPtr tempUsername = std::make_shared<SocketAddress>(incomingAddress);
	usernameMap[tempUsername] = receivedMsg;

	LOG("Accepted connection from %s", usernameMap[tempUsername].c_str());


	bool quit = false;
	std::string msg;

	std::thread receiveThread([&]() { // don't use [&] :)
		while (!quit) // Need to add a quit here to have it really exit!
		{
			//std::cout << "waiting to receive a message" << std::endl;
			int32_t bytesReceived = connSocket->Receive(buffer, 4096);
			if (bytesReceived == 0)
			{
				if (msg == EXIT_CODE)
				{
					std::cout << "server terminating on server's behalf" << std::endl;
					ExitProcess(1);
				}
				LOG("%s has disconnected", usernameMap[tempUsername].c_str());
				usernameMap.erase(tempUsername);

				//TCPSocketPtr connSocket = listenForConnection(incomingAddress, listenSocket);
				LOG("%s", "Waiting to accept connections...");
				connSocket = listenSocket->Accept(incomingAddress);
				while (connSocket == nullptr)
				{
					connSocket = listenSocket->Accept(incomingAddress);
					// SocketUtil::ReportError("Accepting connection");
					// ExitProcess(1);
				}

				connSocket->Send(username.c_str(), username.length());

				char buffer[4096];
				int32_t bytesReceived = connSocket->Receive(buffer, 4096);
				std::string receivedMsg(buffer, bytesReceived);
				tempUsername = std::make_shared<SocketAddress>(incomingAddress);
				usernameMap[tempUsername] = receivedMsg;

				LOG("Accepted connection from %s", usernameMap[tempUsername].c_str());
				quit = false;
			}
			else
			{
				if (bytesReceived < 0)
				{
					SocketUtil::ReportError("Receiving");
					return;
				}
				std::string receivedMsg(buffer, bytesReceived);
				std::map<SocketAddressPtr, std::string>::iterator receivedUsername = usernameMap.find(tempUsername);
				if (receivedUsername == usernameMap.end())
				{
					std::cout << "problem with map" << std::endl;
					ExitProcess(1);
				}
				LOG("Received message from %s: %s", receivedUsername->second.c_str(), receivedMsg.c_str());
			}
		}
		});

	getline(std::cin, msg);
	while (msg != EXIT_CODE)
	{
		//std::cout << "sending a message" << std::endl;
		if (connSocket->Send(msg.c_str(), msg.length()) < 0)
		{
			SocketUtil::ReportError("Problem with sending message");
			ExitProcess(1);
		}
		getline(std::cin, msg);
	}

	std::cout << "exit code entered, quit is true" << std::endl;
	quit = true;
	std::cout << "closing socket" << std::endl;
	connSocket->CloseSocket();
	receiveThread.join();
}

void DoTcpClient(std::string port)
{
	std::map<SocketAddressPtr, std::string> usernameMap;
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
	SocketAddress server = *servAddress;
	//std::cout << server.ToString().c_str() << std::endl;
	if (servAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	std::string username;
	std::cout << "Please input your user name: ";
	getline(std::cin, username);
	//usernameMap.insert(std::pair<SocketAddressPtr, std::string>(clientAddress, username));
	usernameMap[clientAddress] = username;
	//std::cout << usernameMap[clientAddress] << std::endl;

	if (clientSocket->Connect(*servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}

	clientSocket->Send(username.c_str(), username.length());

	LOG("%s", "Connected to server!");

	char buffer[4096];
	int32_t bytesReceived = clientSocket->Receive(buffer, 4096);
	std::string receivedMsg(buffer, bytesReceived);
	usernameMap[servAddress] = receivedMsg;

	bool quit = false;
	std::string msg;
	std::thread receiveThread([&]() { // don't use [&] :)
		while (!quit) // Need to add a quit here to have it really exit!
		{
			//std::cout << "waiting for a message" << std::endl;
			int32_t bytesReceived = clientSocket->Receive(buffer, 4096);
			if (bytesReceived == 0)
			{
				if (msg == EXIT_CODE)
				{
					std::cout << "client terminating on client's behalf" << std::endl;
					ExitProcess(1);
				}
				std::cout << "Server has disconnected, shutting down";
				clientSocket->CloseSocket();
				ExitProcess(1);
			}
			if (bytesReceived < 0)
			{
				SocketUtil::ReportError("Receiving");
				ExitProcess(1);
			}

			std::string receivedMsg(buffer, bytesReceived);
			/*SocketAddressPtr tempUsername = std::make_shared<SocketAddress>(server);
			std::map<SocketAddressPtr, std::string>::iterator receivedUsername = usernameMap.find(tempUsername);
			if (receivedUsername == usernameMap.end())
			{
				std::cout << "problem with map" << std::endl;
				ExitProcess(1);
			}*/
			//username = receivedUsername->second;
			//std::cout << username << std::endl;
			LOG("Received message from %s: %s", usernameMap[servAddress].c_str(), receivedMsg.c_str());
		}
		});

	getline(std::cin, msg);
	while (msg != EXIT_CODE)
	{
		//std::cout << "sending a message" << std::endl;
		if (clientSocket->Send(msg.c_str(), msg.length()) < 0)
		{
			SocketUtil::ReportError("Problem with sending message");
			ExitProcess(1);
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
		getline(std::cin, msg);
	}

	std::cout << "exit code entered, quitting" << std::endl;
	quit = true;
	std::cout << "closing socket" << std::endl;
	clientSocket->CloseSocket();
	receiveThread.join();
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

TCPSocketPtr listenForConnection(SocketAddress incomingAddress, TCPSocketPtr listenSocket)
{
	LOG("%s", "Waiting to accept connections...");
	TCPSocketPtr connSocket = listenSocket->Accept(incomingAddress);
	while (connSocket == nullptr)
	{
		connSocket = listenSocket->Accept(incomingAddress);
		// SocketUtil::ReportError("Accepting connection");
		// ExitProcess(1);
	}

	LOG("Accepted connection from %s", incomingAddress.ToString().c_str());
	return connSocket;
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