#include "RoboCatPCH.h"
#include <iostream>
#include <thread>

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

void DoTcpServer()
{
	// Open a TCP socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listen socket");
		ExitProcess(1);
	}

	LOG("%s", "Created listen socket");

	//SocketAddress a2(INADDR_LOOPBACK, 8080);
	// Listen only for connections on this machine
	SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	if (a == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	LOG("%s", "Create socket address");

	// "Bind" that socket to the address we want to listen on
	if (listenSocket->Bind(*a) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding socket");
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
	// and then it accepts that connection!


	/// listenSocket <----- packets from other hosts wanting to connect
	/// listenSocket -----> accepts connection, spawns:
	///   connectionSocket <----> otherHost1
	///   connectionSock2  <----> otherHost2

	SocketAddress connAddr;
	TCPSocketPtr conn;


	//while (conn == nullptr)
	{
		conn = listenSocket->Accept(connAddr);
	}

	// This code isn't blocking anymore -- it'll run to the end of the program.

	char buffer[4096];
	int32_t bytesRead = conn->Receive(buffer, 4096);

	std::string msgReceived(buffer, bytesRead);
	LOG("Received message from %s: %s", connAddr.ToString().c_str(), msgReceived.c_str());


	std::string sentMsg("Hello, client! I am well.");
	conn->Send(sentMsg.c_str(), sentMsg.length());
	LOG("Attempt to send: %s", sentMsg.c_str());
}

void DoTcpClient()
{
	// Open a TCP socket
	TCPSocketPtr connSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (connSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listen socket");
		ExitProcess(1);
	}

	LOG("%s", "Created client socket");

	char buffer[4096];
	

	//std::thread t(connSocket->Receive(buffer, 4096));
	//t.join();
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

	std::string msg("Hello, server! How are you today?");
	connSocket->Send(msg.c_str(), msg.length());

	LOG("%s", "Sent message to server");


	int32_t bytesRead = connSocket->Receive(buffer, 4096);
	std::string msgReceived(buffer, bytesRead);
	LOG("Received message: %s",  msgReceived.c_str());

	//LOG("Thread: %s", t.get_id());
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
	//   https://docs.microsoft.com/en-us/windows/win32/api/_winsock/

	SocketUtil::StaticInit();

	// Server/Client
	//    (Actually, we want Peer-To-Peer, but we'll get there!)

	// Server Code --------------
	bool isServer = StringUtils::GetCommandLineArg(1) == "server";
	if (isServer)
	{
		DoTcpServer();
	}

	// Client Code --------------
	if (!isServer)
	{
		DoTcpClient();
	}

	SocketUtil::CleanUp();


	system("pause");
	return 0;
}