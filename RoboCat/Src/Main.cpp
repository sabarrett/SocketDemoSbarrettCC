
#include "RoboCatPCH.h"

#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <mutex>

#if _WIN32

//Done using notes from class on multithreading
void DoTcpServer()
{
	// Create socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}

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

	//Handle Sending, using notes on threading from class
	bool sQuit = false;
	std::cout << "Type /disconnect at anypoint to disconnect! /n";
	std::thread sendThreadServer([&]()
		{
			while (!sQuit)
			{
				std::string msg;
				std::getline(std::cin, msg);

				if (msg == "/disconnect")
				{
					sQuit = true;
					connSocket->CleanUp();
					break;
				}
				else
				{
					connSocket->Send(msg.c_str(), msg.length());
				}

			}
		});


	//Handle Recieving, using notes on threading from class
	bool rQuit = false;
	std::thread receiveThreadServer([&]() {
		while (!rQuit)
		{
			char buffer[4096];
			int32_t bytesReceived = connSocket->Receive(buffer, 4096);
			if (bytesReceived < 0)
			{
				SocketUtil::ReportError("Received Nothing");
				return;
			}
			std::string receivedMsg(buffer, bytesReceived);
			std::cout.clear();
			//std::string textOutput("Client %s: %s", incomingAddress.ToString().c_str(), receivedMsg.c_str());
			std::cout << "Client " << incomingAddress.ToString() << ": " << receivedMsg << std::endl;
		}
		});

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
	std::cout << "Type /disconnect at anypoint to disconnect! /n";

	//This part I think is correct, aka it works but I don't think it is best way to handle it
	//Handle Sending, using notes on threading from class
	bool sQuit = false;
	std::thread sendThreadClient([&]()
		{
			while (!sQuit)
			{
				std::string msg;
				std::getline(std::cin, msg);

				if (msg == "/disconnect")
				{
					sQuit = true;
					//Try Close Socket Call but can't access
					clientSocket->CleanUp();
					break;
				}
				else
				{

					clientSocket->Send(msg.c_str(), msg.length());
				}

			}
		});


	//Handle Recieving, using notes on threading from class
	bool rQuit = false;
	std::thread receiveThreadClient([&]() {
		while (!rQuit)
		{
			char buffer[4096];
			int32_t bytesReceived = clientSocket->Receive(buffer, 4096);
			//Check to see what was recieved
			if (bytesReceived < 0)
			{
				SocketUtil::ReportError("Receiving nothing");
				return;
			}
			std::string receivedMsg(buffer, bytesReceived);
			std::cout.clear();
			//output
			std::cout << "Client " << servAddress << ": " << receivedMsg << std::endl;
		}
		});

	receiveThreadClient .join();
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

	bool isServer = StringUtils::GetCommandLineArg(1) == "server";

	if (isServer)
	{
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
