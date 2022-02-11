
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>

#if _WIN32

void DoTCPServer()
{
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

	//creating socket that can listen for messages on the following address
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Listening Socket Error");
		ExitProcess(1);
	}

	LOG("%s", "created listen socket");

	//listens for connections on this machine
	SocketAddressPtr addr = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:8080");

	if (addr == nullptr)
	{
		SocketUtil::ReportError("Server Address Creation Error");
		ExitProcess(1);
	}

	LOG("%s", "created server address");

	//binding listening socket to the address to listen effectively
	if (listenSocket->Bind(*addr) != NO_ERROR)
	{
		SocketUtil::ReportError("Socket Bind Error");
		ExitProcess(1);
	}

	LOG("%s", "socket bound");

	//now to start the listening process officially
	if (listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening Error");
		ExitProcess(1);
	}

	LOG("%s", "socket listening");

	//listenSocket takes in packets from other hosts wanting to connect
	//when listenSocket accepts connections
		//connectionSocket <--> otherHost1
		//connectionSocket2 <--> otherHost2

	SocketAddress connectAddr;
	TCPSocketPtr connect;

	connect = listenSocket->Accept(connectAddr);

	char buffer[4096];
	int32_t bytesRead = connect->Receive(buffer, 4096);

	string messageReceived(buffer, bytesRead);
	LOG("Received Message: %s", messageReceived.c_str());
}

void DoTCPClient()
{
	std::thread t(DoTCPServer);
	//t.join();
	
	// Open a TCP socket
	TCPSocketPtr connSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (connSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listen socket");
		ExitProcess(1);
	}

	LOG("%s", "Created client socket");

	//SocketAddress a2(INADDR_LOOPBACK, 8080);
	// Listen only for connections on this machine
	SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8081");
	if (a == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	LOG("%s", "Create socket address");

	// "Bind" that socket to the address we want to listen on
	if (connSocket->Bind(*a) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding socket");
		ExitProcess(1);
	}

	LOG("%s", "Bound socket");

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	if (servAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	if (connSocket->Connect(*servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}

	LOG("%s", "Connected to server!");

	//change this here to be typed in and then sent
	std::string msg("Hello, server! How are you today?");
	connSocket->Send(msg.c_str(), msg.length());

	LOG("%s", "Sent message to server");
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

	std::thread t([&win]()
				  {
					  int msgNo = 1;
					  while (true)
					  {
						  std::this_thread::sleep_for(std::chrono::milliseconds(250));
						  std::string msgIn("~~~auto message~~~");
						  std::stringstream ss(msgIn);
						  ss << msgNo;
						  win.Write(ss.str());
						  msgNo++;
					  }
				  });

	while (true)
	{
		std::string input;
		std::getline(std::cin, input);
		win.WriteFromStdin(input);
	}

	SocketUtil::CleanUp();

	return 0;
}
