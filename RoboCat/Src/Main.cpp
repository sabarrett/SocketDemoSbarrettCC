#include "RoboCatPCH.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <sstream>
#include <vector>

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


void SendToAll(vector<TCPSocketPtr> connSockets, string msg)
{
	for (TCPSocketPtr currentSocket : connSockets)
	{
		currentSocket->Send(msg.c_str(), msg.length());
	}
}

void SendToExceptSender(vector<TCPSocketPtr> connSockets, TCPSocketPtr sender, string msg)
{
	for (TCPSocketPtr currentSocket : connSockets)
	{
		if (!std::count(connSockets.begin(), connSockets.end(), sender))
		{
			currentSocket->Send(msg.c_str(), msg.length());
		}
	}
}

void DoTcpServer()
{

	string username;
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

	bool fullQuit = false;

	while (!fullQuit)
	{
		LOG("%s", "use /exit to exit chat room, /fullExit to close program\n");
		LOG("%s", "Waiting to accept a connection...");

		vector<TCPSocketPtr> connSockets;
		std::thread connectClients([&connSockets, listenSocket]()
		{
				SocketAddress incomingAddress;
				//if (!std::count(connSockets.begin(), connSockets.end(), listenSocket->Accept(incomingAddress)))
				//{
					//connSockets.push_back(listenSocket->Accept(incomingAddress));
					//LOG("Accepted connection from %s", incomingAddress.ToString().c_str());
				//}

				connSockets.push_back(listenSocket->Accept(incomingAddress));
				LOG("Accepted connection from %s", incomingAddress.ToString().c_str());
		});

		string namePrint;
		bool shouldDelete = false;

		bool quit = false;
		std::thread receiveThread([&quit, &connSockets, &namePrint, &shouldDelete]() { // don't use [&] :)
			while (!quit) // Need to add a quit here to have it really exit!
			{
				char buffer[4096];
				for each (TCPSocketPtr currentSocket in connSockets)
				{
					int32_t bytesReceived = currentSocket->Receive(buffer, 4096);

					if (bytesReceived == 0)
					{
						LOG("%s", "User has disconnected");
						quit = true;
						// handle disconnect
					}
					if (bytesReceived < 0)
					{
						SocketUtil::ReportError("Receiving");
						return;
					}

					if (shouldDelete)
					{
						printf("\33[2K\r                                                             \r");
					}

					std::string receivedMsg(buffer, bytesReceived);

					if (receivedMsg == "/exit")
					{
						LOG("%s", "Other user has exited chat room");
						SendToExceptSender(connSockets, currentSocket, "Other user has exited chat room");
						if (connSockets.size() == 0)
						{
							quit = true;
						}
						break;
					}
					else
					{
						LOG("%s", receivedMsg.c_str());
						SendToExceptSender(connSockets, currentSocket, receivedMsg);
						if (shouldDelete)
						{
							printf(namePrint.c_str());
						}
					}
				}
			}
			});

		string msg;

		if (username.length() == 0 && connSockets.size() > 0)
		{
			std::cout << "Enter DisplayName: ";
			std::getline(std::cin, username);
			msg = "\n" + username + " has entered the chat :)";
			//connSocket->Send(msg.c_str(), msg.length());
			SendToAll(connSockets, msg);

			namePrint = username + ": ";
			printf(namePrint.c_str());
		}
		while (!quit && connSockets.size() > 0)
		{
			std::getline(std::cin, msg);

			if (msg == "/exit" || msg == "/Exit")
			{
				quit = true;
				msg = "/exit";
				//connSocket->Send(msg.c_str(), msg.length());
				SendToAll(connSockets, msg);
				break;
			}
			if (msg == "/fullExit" || msg == "/FullExit" || msg == "/full Exit" || msg == "/Full Exit")
			{
				fullQuit = true;
				msg = "/exit";
				//connSocket->Send(msg.c_str(), msg.length());
				SendToAll(connSockets, msg);
				ExitProcess(1);
				break;
			}


			msg = username + ": " + msg;
			//connSocket->Send(msg.c_str(), msg.length());
			SendToAll(connSockets, msg);

			printf(namePrint.c_str());
			shouldDelete = true;
		}
		receiveThread.join();
		connectClients.join();
	}

		//std::cout << "Press enter to exit at any time!\n";
		//std::cin.get();
		//quit = true;
		//connSocket->~TCPSocket(); // Forcibly close socket (shouldn't call destructors like this -- make a new function for it!
		//sendThread.join();
}

void DoTcpClient(std::string port)
{
	string username;
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

	bool fullQuit = false;
	// Connect() -> Connect socket to remote host
	while (!fullQuit)
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
		LOG("\n\n-------You are in %s's chat room-------\n", clientAddress->ToString().c_str());
		LOG("%s", "use /exit to exit chat room, /fullExit to close program\n");

		string namePrint;
		bool shouldDelete = false;

		bool quit = false;
		std::thread receiveThread([&quit, &clientSocket, &namePrint, &shouldDelete]() { // don't use [&] :)
			while (!quit) // Need to add a quit here to have it really exit!
			{
				char buffer[4096];
				int32_t bytesReceived = clientSocket->Receive(buffer, 4096);
				if (bytesReceived == 0)
				{
					LOG("%s", "Other user has disconnected");
					//LOG("%s", "0 bytes recieved, still going");
					quit = true;
					// handle disconnect
				}
				if (bytesReceived < 0)
				{
					SocketUtil::ReportError("Receiving");
					return;
				}

				if (shouldDelete)
				{
					printf("\33[2K\r                                                             \r");
				}
				std::string receivedMsg(buffer, bytesReceived);

				if (receivedMsg == "/exit")
				{
					LOG("%s", "Host user has exited chat room");
					quit = true;
					break;
				}
				else if(bytesReceived > 0)
				{
					LOG("%s", receivedMsg.c_str());
					if (shouldDelete)
					{
						printf(namePrint.c_str());
					}
				}
			}
			});
		

		if (username.length() == 0)
		{
			std::cout << "Enter DisplayName: ";
			std::getline(std::cin, username);
			string msg = "\n" + username + " has entered the chat :)";
			clientSocket->Send(msg.c_str(), msg.length());
		}

		namePrint = username + ": ";
		printf(namePrint.c_str());

		while (!quit)
		{
			string msg;
			std::getline(std::cin, msg);

			if (msg == "/exit" || msg == "/Exit")
			{
				quit = true;
				msg = "/exit";
				clientSocket->Send(msg.c_str(), msg.length());
				break;
			}
			if (msg == "/fullExit" || msg == "/FullExit" || msg == "/full Exit" || msg == "/Full Exit")
			{
				fullQuit = true;
				msg = "/exit";
				clientSocket->Send(msg.c_str(), msg.length());
				ExitProcess(1);
				break;
			}

			msg = username + ": " + msg;
			clientSocket->Send(msg.c_str(), msg.length());

			printf(namePrint.c_str());
			shouldDelete = true;

		}
		receiveThread.join();
	}

		//std::cout << "Press enter to exit at any time!\n";
		//std::cin.get();
		//quit = true;
		//clientSocket->~TCPSocket(); // Forcibly close socket (shouldn't call destructors like this -- make a new function for it!
		//receiveThread.join();
		//sendThread.join();
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
