
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>

std::vector<TCPSocketPtr> incomingConnections;
std::vector<TCPSocketPtr> outgoingConnections;

struct Connection
{
	TCPSocketPtr inSocketPtr;
	TCPSocketPtr outSocketPtr;
	std::string alias;
};

std::vector<Connection> connections;

bool quit = false;

std::string alias = "anonymous";

std::string receivingAddress;
int receivingPort;

TCPSocketPtr OpenConnection(std::string address, std::string port, bool theyInitiated)
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
	int localPort = 50;
	while (!socketBound && localPort <= 60)
	{
		std::string IPAddr = StringUtils::Sprintf("%s:%d", "127.0.0.1", localPort);
		SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(IPAddr);
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
			LOG("Bound client socket to %s", IPAddr.c_str());
		}
	}

	// Connect() -> Connect socket to remote host
	std::string serverAddressString = StringUtils::Sprintf("%s:%s", address.c_str(), port.c_str());
	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString(serverAddressString);
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

	std::string returnAddress = StringUtils::Sprintf("/RETURNCONNECTION %s %d", receivingAddress.c_str(), receivingPort);
	clientSocket->Send(returnAddress.c_str(), returnAddress.length());

	std::string msg = StringUtils::Sprintf("%s is here!", alias.c_str());
	clientSocket->Send(msg.c_str(), msg.length());

	if (!theyInitiated)
	{
		Connection Connection;
		Connection.outSocketPtr = clientSocket;
		connections.push_back(Connection);
	}
	return clientSocket;
}

void DoTcpServer()
{
	// Create socket
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
	LOG("%s", "Listening on socket");

	//SocketAddress incomingAddress;
	//TCPSocketPtr connSocket; 
	//connSocket->SetNonBlockingMode(true);
	//while (!quit)
	//{
	//	connSocket = listenSocket->Accept(incomingAddress);
	//	if (connSocket != nullptr)
	//	{
	//		incomingConnections.push_back(connSocket);
	//		LOG("Accepted connection from %s", incomingAddress.ToString().c_str());
	//	}
	//}

	LOG("%s", "Waiting to accept connections...");
	SocketAddress incomingAddress;
	TCPSocketPtr connSocket = listenSocket->Accept(incomingAddress);
	while (connSocket == nullptr)
	{
		connSocket = listenSocket->Accept(incomingAddress);
		// SocketUtil::ReportError("Accepting connection");
		// ExitProcess(1);
	}
	Connection connection;
	connection.inSocketPtr = connSocket;
	char buffer[4096];
	int32_t bytesReceived = connection.inSocketPtr->Receive(buffer, 4096);
	std::string receivedMsg(buffer, bytesReceived);
	char* inputC = new char[receivedMsg.length() + 1];
	strcpy(inputC, receivedMsg.c_str());
	char delim[] = " ";
	char* ptr = strtok(inputC, delim);
	while (ptr != NULL)
	{
		if (strcmp(ptr, "/RETURNCONNECTION") == 0)
		{
			ptr = strtok(NULL, delim);
			std::string newAddress(ptr);

			ptr = strtok(NULL, delim);
			std::string newPort(ptr);

			connection.outSocketPtr = OpenConnection(newAddress, newPort, true);
		}
		ptr = NULL;
	}

	//Connection.outSocketPtr = OpenConnection();
	connections.push_back(connection);
	//incomingConnections.push_back(connSocket);
	LOG("Accepted connection from %s", incomingAddress.ToString().c_str());
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

	//DoTcpServer();

	OutputWindow win;
	std::thread t1([&win]()
				  {
					  DoTcpServer();
					  int msgNo = 1;
					  /*while (!quit)
					  {
						  std::this_thread::sleep_for(std::chrono::milliseconds(250));
						  std::string msgIn("noQuit");
						  std::stringstream ss(msgIn);
						  ss << msgNo;
						  win.Write(ss.str());
						  msgNo++;
					  }*/
				  });

	std::thread t2([&win]()
		{
			// Write all received messages
			while (!quit)
			{
				for (const auto& connection : connections)
				{
					if (connection.inSocketPtr != nullptr)
					{
						char buffer[4096];
						int32_t bytesReceived = connection.inSocketPtr->Receive(buffer, 4096);
						while (bytesReceived < 0)
						{
							bytesReceived = connection.inSocketPtr->Receive(buffer, 4096);
						}
						std::string receivedMsg(buffer, bytesReceived);
						if (connection.alias.length() > 0)
						{
							LOG("%s: %s", connection.alias.c_str(), receivedMsg.c_str());
						}
						LOG("Unkown: %s", receivedMsg.c_str());
					}
				}
			}
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

				OpenConnection(newAddress, newPort, false);

				ptr = NULL;
			}
			else if (ptr != NULL && strcmp(ptr, "/quit") == 0)
			{
				quit = true;
				ptr = NULL;
			}
			else
			{
				for (const auto& connection : connections)
				{
					if (connection.outSocketPtr != nullptr)
					{
						connection.outSocketPtr->Send(input.c_str(), input.length());
					}
				}
			}
			ptr = NULL;
		}

		
	}

	SocketUtil::CleanUp();

	return 0;
}

