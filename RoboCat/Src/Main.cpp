
#include "RoboCatPCH.h"
#include <iostream>
#include <thread>

// getInput() (but don't block)
//     check sockets -- did anyone say anything?
//         conn->Receive(); !!!!!! <- blocks!!!
//     Solution: non-blocking mode
//         if (conn.Receive() == -10035)
//             skip this frame; Receive would block
//
// main
// |    spawnThread() --------> netThread
// |    getInput();             | block on Accept(); ....
// |    update();               | spawnThread() ----------->    connThread
// |    goto beginning;         | loop();                       |  conn->Receive(); (blocks)
// |                            |                               |
// |
// v
// 
// Select();
// 
// update();
// renderer();
// repeat();

void DoTCPServer()
{
	// Open a TCP Socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}
	listenSocket->SetNonBlockingMode(false);

	LOG("%s", "Created listen socket");

	// SocketAddress a2(INADDR_LOOPBACK, 8080);

	// Listen only for connections on this machine
	SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");

	if (a == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	LOG("%s", "Created server address");

	// "Bind" that socket to the address we want to listen on

	if (listenSocket->Bind(*a) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding Socket");
		ExitProcess(1);
	}

	LOG("%s", "Bound socket");

	// Call "Listen" to have the OS listen for connections on that socket

	if (listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening");
		ExitProcess(1);
	}

	LOG("%s", "Socket listening");

	// Call "Accept" which *blocks* until we get a request to connect,
	// and then it accepts the connection

	// listenSocket <----- packets from other hosts wanting to connect
	// listenSocket -----> accpets connections, spawns:
	//     connectionSocket <-----> otherHost1
	//     connectionSock2 <-----> otherHost2

	SocketAddress connAddr;
	TCPSocketPtr conn;
	
	conn = listenSocket->Accept(connAddr);

	while (conn = nullptr)
	{
		conn = listenSocket->Accept(connAddr);
	}

	// This code isn't blocking anymore -- it'll run to the end of the program
	
	char buffer[4096];
	int32_t bytesRead = conn->Receive(buffer, 4096);

	std::string msgReceived(buffer, bytesRead);
	LOG("Received message: %s", msgReceived.c_str());
}

void DoTCPClient()
{
	// Open a TCP Socket
	TCPSocketPtr connSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

	if (connSocket == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
	}

	LOG("%s", "Created client socket");

	// SocketAddress a2(INADDR_LOOPBACK, 8080);

	// Listen only for connections on this machine
	SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8081");

	if (a == nullptr)
	{
		SocketUtil::ReportError("Creating socket address");
		ExitProcess(1);
	}

	LOG("%s", "Created socket address");

	// "Bind" that socket to the address we want to listen on

	if (connSocket->Bind(*a) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding Socket");
		ExitProcess(1);
	}

	LOG("%s", "Bound socket");

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");

	if (servAddress == nullptr)
	{
		SocketUtil::ReportError("Create server address");
		ExitProcess(1);
	}
	
	if (connSocket->Connect(*servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Conencting to server");
		ExitProcess(1);
	}

	LOG("%s", "Connected to server!");

	std::string msg("Hello, server! How are you today?");
	connSocket->Send(msg.c_str(), msg.length());
}

#include <thread>
#include <iostream>
#include <string>
#include <sstream>

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

	SocketUtil::StaticInit();

	// Server/Client
	// We want peer-to-peer, but we'll get there!

	// Server Code ----------------
	
	//bool isServer = StringUtils::GetCommandLineArg(1) == "server";

	if (StringUtils::GetCommandLineArg(1) == "server")
	{
		DoTCPServer();
	}

	// Client Code ----------------

	if (StringUtils::GetCommandLineArg(1) == "client")
	{
		DoTCPClient();
	}
	
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
