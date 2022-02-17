
#include "RoboCatPCH.h"
#include <iostream>
#include <cstring>
#include <vector>
#include <thread>

void print(string str)
{
	std::cout << str << std::endl;
}

void Connect(string inputAddress, string serverAddr, std::vector<TCPSocketPtr> &outgoingClientSockets)
{
	// Create socket
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
		return;
	}

	string address = inputAddress;
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError("Creating client address");
		ExitProcess(1);
		return;
	}

	if (clientSocket->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding client socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound client socket");

	// Connect() -> Connect socket to remote host

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString(serverAddr);
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
	else
	{
		outgoingClientSockets.push_back(clientSocket);
		print("Yay");
	}


	LOG("%s", "Connected to server!");

	std::string msg("Hello server! How are you?");
	clientSocket->Send(msg.c_str(), msg.length());
	clientSocket->SetNonBlockingMode(true);

}


void StartServer(bool& listenStatus, std::vector<TCPSocketPtr>& incomingClientSockets, TCPSocketPtr &listenSocket, string addr)
{


	// Create socket
	listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}

	//listenSocket->SetNonBlockingMode(true);

	LOG("%s", "Listening socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString(addr);
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
	
	listenSocket->SetNonBlockingMode(true);


	std::thread listenThread([&listenStatus, &incomingClientSockets, &listenSocket]()
	{
		
		
		
		while (listenStatus)
		{
		
			SocketAddress incomingAddress;
			TCPSocketPtr connSocket = listenSocket->Accept(incomingAddress);
			//connSocket = listenSocket->Accept(incomingAddress);
			// SocketUtil::ReportError("Accepting connection");
			// ExitProcess(1);

			if (connSocket != nullptr) 
			{
				incomingClientSockets.push_back(connSocket);
				LOG("Accepted connection from %s", incomingAddress.ToString().c_str());
				
				
			}
		}

		listenSocket->~TCPSocket();
		LOG("Terminating listen thread %s", " ");
	}
	);

	listenThread.detach();
	
	
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
	string exit = "/exit";
	string help = "/help";
	string connect = "/connect";
	string listen = "/listen";


	bool running = true;
	bool listenStatus = false;
	std::vector<TCPSocketPtr> incomingClientSockets;
	std::vector<TCPSocketPtr> outgoingClientSockets;

	TCPSocketPtr listenSocket;
	

	std::thread receiveThread([&running, &incomingClientSockets, &outgoingClientSockets]()
	{
		while (running)
		{

			std::vector<TCPSocketPtr> deadSockets;

			for (TCPSocketPtr socket : incomingClientSockets)
			{

				if (socket == nullptr)
				{
					deadSockets.push_back(socket);
					break;
				}

				socket->SetNonBlockingMode(true);
				char buffer[4096];
				int32_t bytesReceived = socket->Receive(buffer, 4096);
				if (bytesReceived > 0)
				{
					std::string receivedMsg(buffer, bytesReceived);
					LOG("Received message: %s", receivedMsg.c_str());
					//socket->SetNonBlockingMode(true);
				}
				else  if (bytesReceived < 0)
				{
					if (bytesReceived == -10035) {

					}
					else if (bytesReceived == -10054)
					{
						deadSockets.push_back(socket);
						LOG("User disconnected: %s", "<Insert ID here>");
					}

				}
				
			}

			while(deadSockets.size() > 0){
				TCPSocketPtr socket = deadSockets[deadSockets.size() - 1];
				for (std::vector<TCPSocketPtr>::iterator it = incomingClientSockets.begin(); it != incomingClientSockets.end(); it++)
				{
					if (*it == nullptr)
					{
						incomingClientSockets.erase(it);
						break;
					}
					if (*it == socket)
					{

						incomingClientSockets.erase(it);
						socket->~TCPSocket();
						deadSockets.pop_back();
						break;
					}
				}
			}
			
			
		

			for (TCPSocketPtr socket : outgoingClientSockets)
			{

				if (socket == nullptr)
				{
					deadSockets.push_back(socket);
					break;
				}

				socket->SetNonBlockingMode(true);
				char buffer[4096];
				int32_t bytesReceived = socket->Receive(buffer, 4096);
				if (bytesReceived > 0)
				{
					std::string receivedMsg(buffer, bytesReceived);
					LOG("Received message: %s", receivedMsg.c_str());
					//LOG("Received message from %s: %s", socket->GetAddress(), receivedMsg.c_str());
					//socket->SetNonBlockingMode(true);
				}
				else  if (bytesReceived < 0)
				{
					if (bytesReceived == -10035) {

					}
					else if (bytesReceived == -10054) 
					{
						deadSockets.push_back(socket);
						LOG("User disconnected: %s", "<Insert ID here>");
					}
				}
				
			}

			while (deadSockets.size() > 0) 
			{

				TCPSocketPtr socket = deadSockets[deadSockets.size() - 1];

				for (std::vector<TCPSocketPtr>::iterator it = outgoingClientSockets.begin(); it != outgoingClientSockets.end(); it++)
				{
					if (*it == nullptr)
					{
						incomingClientSockets.erase(it);
						break;
					}
					if (*it == socket)
					{

						incomingClientSockets.erase(it);
						socket->~TCPSocket();
						deadSockets.pop_back();
						break;
					}
				}
			}


			//LOG("Connections: %d", incomingClientSockets.size() + outgoingClientSockets.size());

		}	

		LOG("Terminating receive thread %s", " ");
		
	}
	);


	string input = "";
	while (running)
	{

		
		std::cin >> input;

		if (input[0] == '/')
		{

			if (input == exit)
			{
				running = false;
				listenStatus = false;
			}
			else if (input == help)
			{
				print("Helping...");
			}
			else if (input == connect)
			{
				string localAddress;
				std::cin >> localAddress;

				string serverAddr;

				std::cin >> serverAddr;

				if (serverAddr.length() < 14)
				{
					serverAddr = "127.0.0.1:8080";
				}

				Connect(localAddress, serverAddr, outgoingClientSockets);
			}
			else if (input == listen)
			{
				std::cin >> input;
				if (input == "off")
				{
					listenStatus = false;
				}
				else if(input == "on")
				{

					string addr;

					std::cin >> addr;
					
					if (addr.length() < 14)
					{
						addr = "127.0.0.1:8080";
					}
					if (!listenStatus)
					{
						listenStatus = true;
						StartServer(listenStatus, incomingClientSockets, listenSocket, addr);
					}
				}
				
			}
			else
			{
				print("Invalid command...");
			}

		}
		else
		{
			for (TCPSocketPtr socket : incomingClientSockets)
			{
				
				socket->Send(input.c_str(), input.length());
				
			}

			for (TCPSocketPtr socket : outgoingClientSockets)
			{
				
				socket->Send(input.c_str(), input.length());
				
			}
		}
		
	}


	receiveThread.join();
	
	SocketUtil::CleanUp();

	return 0;
}



