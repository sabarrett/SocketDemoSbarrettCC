
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>
#include <mutex>

#if _WIN32

const int MIN_PORT = 8080;
const int MAX_PORT = 65535;

std::mutex mtx;

void DoTcpPeer()
{
	// Create peer socket
	TCPSocketPtr peerSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (peerSocket == nullptr)
	{
		SocketUtil::ReportError("Unable to create a peer socket");
		ExitProcess(1);
	}
	LOG("%s", "Peer socket created");

	int peerTCPPort = MIN_PORT - 1;

	std::string address;
	SocketAddressPtr peerAddress;

	do
	{
		peerTCPPort++;
		address = StringUtils::Sprintf("127.0.0.1:%s", std::to_string(peerTCPPort).c_str());
		peerAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());

	} while (peerSocket->Bind(*peerAddress) != NO_ERROR);
	LOG("Peer address created: %s", peerAddress->ToString().c_str());

	peerSocket->SetNonBlockingMode(false);

	std::list<SocketAddressPtr> otherPeersAdressesList;
	std::list<TCPSocketPtr> otherPeersSockets;

	// Try connecting to other peers
	for (int portToCheck = MIN_PORT; portToCheck < peerTCPPort; portToCheck++)
	{
		std::cout << "Test1" << std::endl;

		address = StringUtils::Sprintf("127.0.0.1:%s", std::to_string(portToCheck).c_str());
		SocketAddressPtr anotherPeerAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
		if (anotherPeerAddress == nullptr)
		{
			SocketUtil::ReportError("Unable to create server address");
			ExitProcess(1);
		}


		if (peerSocket->Connect(*anotherPeerAddress) != NO_ERROR)
		{
			SocketUtil::ReportError("No connection to port ");
			std::cout << anotherPeerAddress->ToString().c_str() << std::endl;
		}
		else
		{
			otherPeersAdressesList.push_back(anotherPeerAddress);
			LOG("Connected to peer with address: %s", anotherPeerAddress->ToString().c_str());
		}
	}

	peerSocket->SetNonBlockingMode(true);

	// Start listening for incoming connections
	if (peerSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Unable to start listening on this socket");
	}
	LOG("%s", "Listening on socket");

	bool quit = false;

	std::cout << "Test3" << std::endl;

	std::thread acceptThread([&quit, &peerSocket, &otherPeersSockets]()
		{
			while (!quit)
			{
				mtx.lock();
				//std::cout << "1) accept Thread\n";

				SocketAddress anotherPeerAddress;
				TCPSocketPtr anotherPeerSocket;
				
				anotherPeerSocket = peerSocket->Accept(anotherPeerAddress);

				if (anotherPeerSocket != nullptr)
				{
					otherPeersSockets.push_back(anotherPeerSocket);
					LOG("Established connection with %s", anotherPeerAddress.ToString().c_str());
				}
				mtx.unlock();
			}
		});

	std::thread receiveThread([&quit, &peerSocket, &otherPeersSockets]()
		{

			while (!quit)
			{
				//std::cout << "2) receive Thread\n";

				for (TCPSocketPtr otherSocket : otherPeersSockets)
				{
					char buffer[4096];

					int32_t bytesReceived = otherSocket->Receive(buffer, 4096);
					if (bytesReceived == 0)
					{
						SocketUtil::ReportError("Connection gracefully closed");

						otherPeersSockets.remove(otherSocket);

						continue;
					}
					if (bytesReceived < 0)
					{
						SocketUtil::ReportError("Connection forcefully closed");

						otherPeersSockets.remove(otherSocket);

						continue;
					}

					std::string receivedMsg(buffer, bytesReceived);
					LOG("Received message: %s", receivedMsg.c_str());
				}
			}
		});

	std::thread sendThread([&quit, &peerSocket]()
		{
			while (!quit)
			{
				std::string msg;
				std::cin >> msg;
				
				if (msg != "/exit")
					peerSocket->Send(msg.c_str(), msg.length());
				else
					quit = true;
			}
		});

	std::cout << "Type '/exit' and press Enter to exit!\n";

	while (!quit)
	{
	}

	for (TCPSocketPtr otherSocket : otherPeersSockets)
	{
		otherSocket->~TCPSocket();	// Forcibly close socket (shouldn't call destructors like this -- make a new function for it! //!!!
	}

	sendThread.join();
	acceptThread.join();
	receiveThread.join();
}

void DoTcpServer(OutputWindow& win)
{
	// Create listening socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError(" Unable to create a listening socket");
		ExitProcess(1);
	}
	LOG("%s", " Listening socket created");

	// Create listening address
	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:8080");
	if (listenAddress == nullptr)
	{
		SocketUtil::ReportError(" Unable to create a listen address");
		ExitProcess(1);
	}

	// Bind address to socket
	if (listenSocket->Bind(*listenAddress) != NO_ERROR)
	{
		SocketUtil::ReportError(" Unable to bind listening socket");
		ExitProcess(1);
	}
	LOG("%s", " Bound listening socket");

	// Start listening for incoming connections
	if (listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError(" Unable to start listening on a listening socket");
		ExitProcess(1);
	}
	LOG("%s", " Listening on socket");

	// Wait for incoming connection
	SocketAddress incomingAddress;
	TCPSocketPtr connSocket = listenSocket->Accept(incomingAddress);
	LOG("%s", " Waiting for connections...");

	while (connSocket == nullptr)
	{
		connSocket = listenSocket->Accept(incomingAddress);
		
		// SocketUtil::ReportError("No incoming connection");
		// ExitProcess(1);
	}
	LOG(" Accepted connection from %s", incomingAddress.ToString().c_str());

	bool quit = false;
	std::thread receiveThread([&quit, &connSocket, &incomingAddress]() {
		while (!quit)
		{
			char buffer[4096];
			int32_t bytesReceived = connSocket->Receive(buffer, 4096);
			if (bytesReceived == 0)
			{
				SocketUtil::ReportError(" Connection gracefully closed");
				return;
			}
			if (bytesReceived < 0)
			{
				SocketUtil::ReportError(" Connection forcefully closed");
				return;
			}

			std::string receivedMsg(buffer, bytesReceived);
			LOG(" Received message from %s: %s", incomingAddress.ToString().c_str(), receivedMsg.c_str());
			std::cout << " WERT" << std::endl;
		}
		});

	std::thread sendThread([&quit, &connSocket, &win]()
		{
			while (!quit)
			{
				std::string input;
				std::getline(std::cin, input);
				win.WriteFromStdin(input);

				if (input != "/exit")
				{
					connSocket->Send(input.c_str(), input.length());
				}
				else
				{
					quit = true;
				}
			}
		});

	std::cout << " Type '/exit' and press Enter to exit!\n";

	while (!quit)
	{

	}

	connSocket->Close(); // Forcibly close socket (shouldn't call destructors like this -- make a new function for it! //!!!
	sendThread.join();
	receiveThread.join();
}

void DoTcpClient(std::string port, OutputWindow& win)
{
	// Create socket for client
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
	{
		SocketUtil::ReportError(" Unable to create a client socket");
		ExitProcess(1);
	}
	LOG("%s", " Client socket created");

	// Create address for client
	std::string address = StringUtils::Sprintf("127.0.0.1:%s", port.c_str());
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError(" Unable to create client address");
		ExitProcess(1);
	}

	// Bind address to socket
	if (clientSocket->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError(" Unable to bind client socket");
		ExitProcess(1);
	}
	LOG("%s", " Bound client socket");

	// Create address for server
	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	if (servAddress == nullptr)
	{
		SocketUtil::ReportError(" Unable to create server address");
		ExitProcess(1);
	}

	// Connect socket to remote host
	if (clientSocket->Connect(*servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError(" No connection to server");
		ExitProcess(1);
	}
	LOG("%s", " Connected to server!");

	bool quit = false;
	std::thread receiveThread([&quit, &clientSocket, &servAddress]() {
		while (!quit)
		{
			char buffer[4096];
			int32_t bytesReceived = clientSocket->Receive(buffer, 4096);
			if (bytesReceived == 0)
			{
				SocketUtil::ReportError(" Connection gracefully closed");
				return;
			}
			if (bytesReceived < 0)
			{
				SocketUtil::ReportError(" Connection forcefully closed");
				return;
			}
			std::string receivedMsg(buffer, bytesReceived);
			LOG(" Received message from %s: %s", servAddress->ToString().c_str(), receivedMsg.c_str());
		}
		});

	std::thread sendThread([&quit, &clientSocket, &win]()
		{
			while (!quit)
			{
				std::string input;
				std::getline(std::cin, input);
				win.WriteFromStdin(input);

				if (input != "/exit")
				{
					clientSocket->Send(input.c_str(), input.length());
				}
				else
				{
					quit = true;
				}
			}
		});

	std::cout << " Type '/exit' and press Enter to exit!\n";

	while (!quit)
	{
	}

	clientSocket->Close();
	sendThread.join();
	receiveThread.join();
}

/*
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
} */

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

	std::thread t([&win]()
				  {
					  int msgNo = 1;
					  while (true)
					  {
						  std::this_thread::sleep_for(std::chrono::milliseconds(250));
						  std::string msgIn("~");
						  std::stringstream ss(msgIn);
						  win.Write(ss.str());
						  msgNo++;
					  }
				  });

	bool isServer = StringUtils::GetCommandLineArg(1) == "server";
	if (isServer)
	{
		DoTcpServer(win);
	}
	else // then client
	{
		//DoTcpPeer();
		DoTcpClient(StringUtils::GetCommandLineArg(2), win);
	}
	std::cout << " 3333333\n";
	SocketUtil::CleanUp();
	std::cout << " 4444444\n";

	return 0;
}
