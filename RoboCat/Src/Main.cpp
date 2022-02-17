
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>

std::vector<TCPSocketPtr> incomingConnections;
std::vector<TCPSocketPtr> outgoingConnections;

struct Connection
{
	TCPSocketPtr socketPtr;
	std::string alias;
};

std::vector<Connection> connections;

//bool quit = false;

std::string alias = "anonymous";

std::string receivingAddress;
int receivingPort;

TCPSocketPtr OpenConnection(std::string address, std::string port)
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
	bool socketBound = false;
	int localPort = 64000;
	while (!socketBound && localPort <= 64100)
	{
		std::string myIPAddr = StringUtils::Sprintf("%s:%d", "127.0.0.1", localPort);
		SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(myIPAddr);
		if (clientAddress == nullptr)
		{
			SocketUtil::ReportError("Creating client address");
		}
		if (clientSocket->Bind(*clientAddress) != NO_ERROR)
		{
			SocketUtil::ReportError("Binding client socket");
			localPort++;
		}
		else
		{
			receivingAddress = "127.0.0.1";
			receivingPort = localPort;
			socketBound = true;
			LOG("Bound client socket to %s", myIPAddr.c_str());
		}
	}

	// Connect() -> Connect socket to remote host
	std::string serverAddressString = StringUtils::Sprintf("%s:%s", address.c_str(), port.c_str());
	SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString(serverAddressString);
	if (serverAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}
	if (clientSocket->Connect(*serverAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}
	LOG("%s", "Connected to server!");

	std::string msg = StringUtils::Sprintf("%s is here!", alias.c_str());
	clientSocket->Send(msg.c_str(), msg.length());

	Connection connection;
	connection.socketPtr = clientSocket;
	connection.socketPtr->SetNonBlockingMode(true);
	connection.alias = alias;
	connections.push_back(connection);

	return clientSocket;
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
	//SocketUtil::CleanUp();
	bool quit = false;
	OutputWindow win;

	std::thread t1([&win, &quit]()
				  {
			// Create listener socket
			TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
			if (listenSocket == nullptr)
			{
				SocketUtil::ReportError("Creating listening socket");
				ExitProcess(1);
			}
			LOG("%s", "Listening socket created");

			// Bind() - "Bind" socket -> tells OS we want to use a specific address
			bool socketBound = false;
			int port = 8080;
			while (!socketBound && port <= 8090)
			{
				std::string IPAddr = StringUtils::Sprintf("127.0.0.1:%d", port);
				SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString(IPAddr);
				if (listenAddress == nullptr)
				{
					SocketUtil::ReportError("Creating listen address");
				}
				if (listenSocket->Bind(*listenAddress) != NO_ERROR)
				{
					SocketUtil::ReportError("Binding listening socket");
					port++;
				}
				else
				{
					receivingAddress = "127.0.0.1";
					receivingPort = port;
					socketBound = true;
					LOG("Bound listening socket to %s", IPAddr.c_str());
				}
			}

			// Listen() - Listen on socket -> Non-blocking; tells OS we care about incoming connections on this socket
			if (listenSocket->Listen() != NO_ERROR)
			{
				SocketUtil::ReportError("Listening on listening socket");
				ExitProcess(1);
			}
			LOG("%s", "Waiting to accept connections...");

			listenSocket->SetNonBlockingMode(true);
			SocketAddress incomingAddress;
			TCPSocketPtr connSocket;// = nullptr;// = listenSocket->Accept(incomingAddress);
			while (!quit)
			{
				connSocket = listenSocket->Accept(incomingAddress);
				/*while (connSocket == nullptr)
				{
					connSocket = listenSocket->Accept(incomingAddress);
				}*/
				if (connSocket != nullptr)
				{
					Connection connection;
					connSocket->SetNonBlockingMode(true);
					connection.socketPtr = connSocket;
					connections.push_back(connection);
					LOG("Accepted connection from %s", incomingAddress.ToString().c_str());
					std::string msg = StringUtils::Sprintf("Connected to %s!", alias.c_str());
					connSocket->Send(msg.c_str(), msg.length());
				}
			}
			listenSocket->CleanUp();
			return;
				  });

	std::thread t2([&win, &quit]()
		{
			// Write all received messages
			while (!quit)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
				for (int i = 0; i < connections.size(); i++)
				{
					if (connections[i].socketPtr != nullptr)
					{
						char buffer[4096];
						int32_t bytesReceived = connections[i].socketPtr->Receive(buffer, 4096);
						if (bytesReceived > 0)
						{
							std::string receivedMsg(buffer, bytesReceived);
							char* inputC = new char[receivedMsg.length() + 1];
							strcpy(inputC, receivedMsg.c_str());
							char delim[] = " ";
							char* ptr = strtok(inputC, delim);
							while (ptr != NULL)
							{
								if (ptr != NULL && strcmp(ptr, "/CHANGEALIAS") == 0)
								{
									ptr = strtok(NULL, delim);
									std::string newAlias(ptr);
									connections[i].alias = newAlias;
								}
								ptr = NULL;
							}
							if (connections[i].alias.length() > 0)
							{
								LOG("%s: %s", connections[i].alias.c_str(), receivedMsg.c_str());
							}
							else
							{
								LOG("Unkown: %s", receivedMsg.c_str());
							}
						}
						else if (bytesReceived == 0 || SocketUtil::GetLastError() == 10054)
						{
							LOG("%s has disconnected", connections[i].alias.c_str());
							connections[i].socketPtr->CleanUp();
							connections.erase(connections.begin() + i);
						}
						
					}
				}
			}
			return;
		});

	while (!quit)
	{
		std::string input;
		std::getline(std::cin, input);
		//win.WriteFromStdin(input);
		win.Write(input);
		char* inputC = new char[input.length() + 1];
		strcpy(inputC, input.c_str());
		char delim[] = " ";

		char* ptr = strtok(inputC, delim);

		while (ptr != NULL)
		{
			if (strcmp(ptr, "/connect") == 0)
			{
				ptr = strtok(NULL, delim);
				std::string newAddress(ptr);

				ptr = strtok(NULL, delim);
				std::string newPort(ptr);

				OpenConnection(newAddress, newPort);

				ptr = NULL;
			}
			else if (ptr != NULL && strcmp(ptr, "/exit") == 0)
			{
				for (int i = 0; i < connections.size(); i++)
				{
					connections[i].socketPtr->CleanUp();
				}
				connections.clear();
				quit = true;
				t1.join();
				t2.join();
				ptr = NULL;
			}
			else if (ptr != NULL && strcmp(ptr, "/alias") == 0)
			{
				ptr = strtok(NULL, delim);
				std::string const newAlias(ptr);
				alias = newAlias;
				for (auto& connection : connections)
				{
					std::string changeAlias = StringUtils::Sprintf("/CHANGEALIAS %s", newAlias.c_str());
					connection.socketPtr->Send(changeAlias.c_str(), changeAlias.length());
				}
				ptr = NULL;
			}
			else
			{
				for (int i = 0; i < connections.size(); i++)
				{
					if (connections[i].socketPtr != nullptr)
					{
						connections[i].socketPtr->Send(input.c_str(), input.length());
					}
				}
			}
			ptr = NULL;
		}
	}

	SocketUtil::CleanUp();

	return 0;
}

