
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>

struct Connection
{
	TCPSocketPtr socketPtr;
	std::string alias;
	int connectionID;
};

std::vector<Connection> connections;

std::string alias = "anonymous"; // <-- Default alias.
int nextID = 1;

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

	// Bind() - "Bind" socket -> bind to address:port - trying ports 64000-64100
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

	// Announce arrival and update alias for other clients
	std::string msg = StringUtils::Sprintf("/CHANGEALIAS %s is here!", alias.c_str());
	clientSocket->Send(msg.c_str(), msg.length());

	// Add to connections list
	Connection connection;
	connection.socketPtr = clientSocket;
	connection.socketPtr->SetNonBlockingMode(true);
	connection.alias = alias;
	connection.connectionID = nextID;
	nextID++;
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
	
	bool quit = false;
	OutputWindow win;

	SocketUtil::StaticInit();

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

			// Bind listener to address:port, trying other ports if 8080 is taken - allows for multiple unique instances on one device.
			bool socketBound = false;
			int port = 8080;
			while (!socketBound && port <= 8090)
			{
				std::string IPAddr = StringUtils::Sprintf("0.0.0.0:%d", port);
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

			// Do non-blocking accept loop while running
			listenSocket->SetNonBlockingMode(true);
			SocketAddress incomingAddress;
			TCPSocketPtr connSocket;
			while (!quit)
			{
				connSocket = listenSocket->Accept(incomingAddress);
				if (connSocket != nullptr)
				{
					Connection connection;
					connSocket->SetNonBlockingMode(true);
					connection.socketPtr = connSocket;
					connection.connectionID = nextID;
					nextID++;
					connections.push_back(connection);
					LOG("-------------Accepted connection from %s-------------", incomingAddress.ToString().c_str());
					std::string msg = StringUtils::Sprintf("-------------Connected to %s!-------------", alias.c_str());
					connSocket->Send(msg.c_str(), msg.length());
				}
			}
			listenSocket->CleanUp();
			return;
				  });

	std::thread t2([&win, &quit]()
		{
			// While running
			while (!quit)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(40)); // Without this sleep, the thread may occasionally begin attempting Receive on a VERY fresh socket(), 
				for (int i = 0; i < connections.size(); i++)				//^throwing an error. The documentation specifies that "some time must pass" between an accept and Receive().
				{
					if (connections[i].socketPtr != nullptr)
					{
						// Write messages from each connection
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
								// Write message
								LOG("%d - %s: %s", connections[i].connectionID, connections[i].alias.c_str(), receivedMsg.c_str());
							}
							else 
							{
								// Write Message with no alias
								LOG("%d - Unkown: %s", connections[i].connectionID, receivedMsg.c_str());
							}
						}
						else if (bytesReceived == 0 || SocketUtil::GetLastError() == 10054)
						{
							// Connection closed, forecfully or otherwise - discard.
							LOG("-------------%s has disconnected-------------", connections[i].alias.c_str());
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

		// Read inputs to check for commands
		// /connect IPAddr PortNumber - connect to a peer -> Peers do not syndicate other peers: To form a chat "room", peers must connect to each person they want to talk to.
		// /disconnect ID - close specified connection
		// /alias - change name
		// /list - shows all active connections by ID and alias
		// /tell ID - sends message to specified ID
		// /exit - disconnect all and close program
		// IF NO COMMAND is detected -> sends typed message to all connections.
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
			else if (ptr != NULL && strcmp(ptr, "/disconnect") == 0)
			{
				ptr = strtok(NULL, delim);
				std::string const disconnectID(ptr);
				for (int i = 0; i < connections.size(); i++)
				{
					if (connections[i].connectionID == std::stoi(disconnectID))
					{
						LOG("-------------Disconnected from %d - %s-------------", connections[i].connectionID, connections[i].alias.c_str());
						connections[i].socketPtr->CleanUp();
						connections.erase(connections.begin() + i);
					}
				}
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
			else if (ptr != NULL && strcmp(ptr, "/list") == 0)
			{
				LOG("-------------Active Connections-------------", 0);
				for (int i = 0; i < connections.size(); i++)
				{
					LOG("%d - %s", connections[i].connectionID, connections[i].alias.c_str());
				}
				LOG("--------------------------------------------", 0);
				ptr = NULL;
			}
			else if (ptr != NULL && strcmp(ptr, "/tell") == 0)
			{
				ptr = strtok(NULL, delim);
				std::string const tellID(ptr);
				for (int i = 0; i < connections.size(); i++)
				{
					if (connections[i].socketPtr != nullptr && connections[i].connectionID == std::stoi(tellID))
					{
						connections[i].socketPtr->Send(input.c_str(), input.length());
					}
				}
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
			else // Send to all by default
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

