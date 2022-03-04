
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>
#include <mutex>

#if _WIN32

std::mutex mtx;

//const int MIN_PORT = 8080;
//const int MAX_PORT = 65535;

///
/// It was supposed to become a multi-peer connection, but now this code is just resting here
///

//void DoTcpPeer()
//{
//	// Create peer socket
//	TCPSocketPtr peerSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
//	if (peerSocket == nullptr)
//	{
//		SocketUtil::ReportError("Unable to create a peer socket");
//		ExitProcess(1);
//	}
//	LOG("%s", "Peer socket created");
//
//	int peerTCPPort = MIN_PORT - 1;
//
//	std::string address;
//	SocketAddressPtr peerAddress;
//
//	do
//	{
//		peerTCPPort++;
//		address = StringUtils::Sprintf("127.0.0.1:%s", std::to_string(peerTCPPort).c_str());
//		peerAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
//
//	} while (peerSocket->Bind(*peerAddress) != NO_ERROR);
//	LOG("Peer address created: %s", peerAddress->ToString().c_str());
//
//	peerSocket->SetNonBlockingMode(false);
//
//	std::list<SocketAddressPtr> otherPeersAdressesList;
//	std::list<TCPSocketPtr> otherPeersSockets;
//
//	// Try connecting to other peers
//	for (int portToCheck = MIN_PORT; portToCheck < peerTCPPort; portToCheck++)
//	{
//		std::cout << "Test1" << std::endl;
//
//		address = StringUtils::Sprintf("127.0.0.1:%s", std::to_string(portToCheck).c_str());
//		SocketAddressPtr anotherPeerAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
//		if (anotherPeerAddress == nullptr)
//		{
//			SocketUtil::ReportError("Unable to create server address");
//			ExitProcess(1);
//		}
//
//
//		if (peerSocket->Connect(*anotherPeerAddress) != NO_ERROR)
//		{
//			SocketUtil::ReportError("No connection to port ");
//			std::cout << anotherPeerAddress->ToString().c_str() << std::endl;
//		}
//		else
//		{
//			otherPeersAdressesList.push_back(anotherPeerAddress);
//			LOG("Connected to peer with address: %s", anotherPeerAddress->ToString().c_str());
//		}
//	}
//
//	peerSocket->SetNonBlockingMode(true);
//
//	// Start listening for incoming connections
//	if (peerSocket->Listen() != NO_ERROR)
//	{
//		SocketUtil::ReportError("Unable to start listening on this socket");
//	}
//	LOG("%s", "Listening on socket");
//
//	bool quit = false;
//
//	std::cout << "Test3" << std::endl;
//
//	std::thread acceptThread([&quit, &peerSocket, &otherPeersSockets]()
//		{
//			while (!quit)
//			{
//				mtx.lock();
//				//std::cout << "1) accept Thread\n";
//
//				SocketAddress anotherPeerAddress;
//				TCPSocketPtr anotherPeerSocket;
//				
//				anotherPeerSocket = peerSocket->Accept(anotherPeerAddress);
//
//				if (anotherPeerSocket != nullptr)
//				{
//					otherPeersSockets.push_back(anotherPeerSocket);
//					LOG("Established connection with %s", anotherPeerAddress.ToString().c_str());
//				}
//				mtx.unlock();
//			}
//		});
//
//	std::thread receiveThread([&quit, &peerSocket, &otherPeersSockets]()
//		{
//
//			while (!quit)
//			{
//				//std::cout << "2) receive Thread\n";
//
//				for (TCPSocketPtr otherSocket : otherPeersSockets)
//				{
//					char buffer[4096];
//
//					int32_t bytesReceived = otherSocket->Receive(buffer, 4096);
//					if (bytesReceived == 0)
//					{
//						SocketUtil::ReportError("Connection gracefully closed");
//
//						otherPeersSockets.remove(otherSocket);
//
//						continue;
//					}
//					if (bytesReceived < 0)
//					{
//						SocketUtil::ReportError("Connection forcefully closed");
//
//						otherPeersSockets.remove(otherSocket);
//
//						continue;
//					}
//
//					std::string receivedMsg(buffer, bytesReceived);
//					LOG("Received message: %s", receivedMsg.c_str());
//				}
//			}
//		});
//
//	std::thread sendThread([&quit, &peerSocket]()
//		{
//			while (!quit)
//			{
//				std::string msg;
//				std::cin >> msg;
//				
//				if (msg != "/exit")
//					peerSocket->Send(msg.c_str(), msg.length());
//				else
//					quit = true;
//			}
//		});
//
//	std::cout << "Type '/exit' and press Enter to exit!\n";
//
//	while (!quit)
//	{
//	}
//
//	for (TCPSocketPtr otherSocket : otherPeersSockets)
//	{
//		otherSocket->~TCPSocket();	// Forcibly close socket (shouldn't call destructors like this -- make a new function for it! //!!!
//	}
//
//	sendThread.join();
//	acceptThread.join();
//	receiveThread.join();
//}

void DoTcpServer(std::string username, std::string portNumber, OutputWindow& win, bool& quit)
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
	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:" + portNumber);
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

	char buffer[4096];
	int32_t bytesReceived = connSocket->Receive(buffer, 4096);
	if (bytesReceived == 0)
	{
		LOG("%s", " Connection forcefully closed. Press enter to exit!");
		quit = true;
		return;
	}
	if (bytesReceived < 0)
	{
		LOG("%s", " Connection gracefully closed. Press enter to exit!");
		quit = true;
		return;
	}

	std::string connectedUsername(buffer, bytesReceived);
	LOG(" User %s joined chat room", connectedUsername.c_str());

	connSocket->Send(username.c_str(), username.length());
	LOG("%s", " Username Sent!");

	std::thread receiveThread([&quit, &connSocket, &connectedUsername]() {
		while (!quit)
		{
			if (connSocket != nullptr)
			{
				char buffer[4096];
				int32_t bytesReceived = connSocket->Receive(buffer, 4096);
				if (bytesReceived == 0)
				{
					LOG("%s", " Connection gracefully closed. Press enter to exit!");
					quit = true;
					connSocket->Close();
					connSocket = nullptr;
					return;
				}
				if (bytesReceived < 0)
				{
					LOG("%s", " Connection forcefully closed. Press enter to exit!");
					quit = true;
					connSocket->Close();
					connSocket = nullptr;
					return;
				}
				std::string receivedMsg(buffer, bytesReceived);
				if (receivedMsg == "---")
				{
					LOG(" User %s left chat room. Press enter to exit!", connectedUsername.c_str());
					quit = true;
					connSocket->Close();
					connSocket = nullptr;
					return;
				}
				else
				{LOG("%s", " Connection gracefully closed. Press enter to exit!");
					LOG(" %s: %s", connectedUsername.c_str(), receivedMsg.c_str());
				}
			}
		}
		});

	std::thread sendThread([&quit, &connSocket, &win]()
		{
			while (!quit)
			{
				if (connSocket != nullptr)
				{
					std::string input;
					std::getline(std::cin, input);
					if (!quit)
						win.WriteFromStdin(" You: " + input);

					if (input != "/exit")
					{
						connSocket->Send(input.c_str(), input.length());
					}
					else
					{
						std::string message("---");
						connSocket->Send(message.c_str(), message.length());

						LOG("%s", " Connection gracefully closed.  Press enter to exit!");
						connSocket->Close();
						connSocket = nullptr;
						quit = true;
					}
				}
			}
		});

	std::cout << " Type '/exit' and press Enter to exit!\n";

	sendThread.join();
	receiveThread.join();
}



void DoTcpClient(std::string username, std::string portNumber, std::string serverPortNumber, OutputWindow& win, bool& quit)
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
	std::string address = StringUtils::Sprintf("127.0.0.1:%s", portNumber.c_str());
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
	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:" + serverPortNumber);
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

	clientSocket->Send(username.c_str(), username.length());
	LOG("%s", " Username Sent!");

	char buffer[4096];
	int32_t bytesReceived = clientSocket->Receive(buffer, 4096);
	if (bytesReceived == 0)
	{
		LOG("%s", " Connection gracefully closed. Press enter to exit!");
		quit = true;
		return;
	}
	if (bytesReceived < 0)
	{
		LOG("%s", " Connection forcefully closed. Press enter to exit!");
		quit = true;
		return;
	}

	std::string connectedUsername(buffer, bytesReceived);
	LOG(" User %s joined chat room", connectedUsername.c_str());

	std::thread receiveThread([&quit, &clientSocket, &connectedUsername]() {
		while (!quit)
		{
			if (clientSocket != nullptr)
			{
				char buffer[4096];
				int32_t bytesReceived = clientSocket->Receive(buffer, 4096);
				if (bytesReceived == 0)
				{
					LOG("%s", " Connection gracefully closed. Press enter to exit!");
					quit = true;
					clientSocket->Close();
					clientSocket = nullptr;
					return;
				}
				if (bytesReceived < 0)
				{
					LOG("%s", " Connection forcefully closed. Press enter to exit!");
					quit = true;
					clientSocket->Close();
					clientSocket = nullptr;
					return;
				}
				std::string receivedMsg(buffer, bytesReceived);
				if (receivedMsg == "---")
				{
					quit = true;
					clientSocket->Close();
					clientSocket = nullptr;
					LOG(" User %s left chat room. Press any button to exit. Press enter to exit!", connectedUsername.c_str());
					return;
				}
				else
				{
					LOG(" %s: %s", connectedUsername.c_str(), receivedMsg.c_str());
				}
			}
		}
		});

	std::thread sendThread([&quit, &clientSocket, &win]()
		{
			while (!quit)
			{
				if (clientSocket != nullptr)
				{
					std::string input;
					std::getline(std::cin, input);

					if(!quit)
						win.WriteFromStdin(" You: " + input);

					if (input != "/exit")
					{
						clientSocket->Send(input.c_str(), input.length());
					}
					else
					{
						std::string message("---");
						clientSocket->Send(message.c_str(), message.length());

						LOG("%s", " Connection gracefully closed. Press enter to exit!");
						quit = true;
						clientSocket->Close();
						clientSocket = nullptr;
					}
				}
			}
		});

	std::cout << " Type '/exit' and press Enter to exit!\n";

	sendThread.join();
	receiveThread.join();
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

	OutputWindow win;
	bool quit = false;

	std::thread t([&win, &quit]()
				  {
					  while (!quit)
					  {
						  std::this_thread::sleep_for(std::chrono::milliseconds(250));
						  std::string msgIn("~");
						  std::stringstream ss(msgIn);
						  win.Write(ss.str());
					  }
				  });

	bool isServer = StringUtils::GetCommandLineArg(1) == "server";
	if (isServer)
	{
		DoTcpServer(StringUtils::GetCommandLineArg(3), StringUtils::GetCommandLineArg(2), win, quit);
	}
	else // then client
	{
		DoTcpClient(StringUtils::GetCommandLineArg(4), StringUtils::GetCommandLineArg(2), StringUtils::GetCommandLineArg(3), win, quit);
	}

	SocketUtil::CleanUp();

	return 0;
}
