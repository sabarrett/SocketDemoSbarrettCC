
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>

void DoTcpServer();
void DoTcpClient();
void sendMessage(TCPSocketPtr socket, string message); 

using namespace std;

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

	OutputWindow win;

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

	return 0;
}

void DoTcpServer()
{

	cout << "Welcome to the AOL!" << endl;
	cout << "Please enter your username: ";
	
	string username;

	cin >> username;

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
	SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:8080");
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
	LOG("Received message: %s", msgReceived.c_str());

	std::string msg("Hi Client! What a swell day!");
	conn->Send(msg.c_str(), msg.length());
}

void DoTcpClient()
{

	cout << "Welcome to the AOL!" << endl;
	cout << "Please enter your username: ";

	string username;

	cin >> username;


	// Open a TCP socket
	TCPSocketPtr connSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (connSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listen socket");
		ExitProcess(1);
	}


	//SocketAddress a2(INADDR_LOOPBACK, 8080);
	// Listen only for connections on this machine
	SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8081");
	if (a == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}


	// "Bind" that socket to the address we want to listen on
	if (connSocket->Bind(*a) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding socket");
		ExitProcess(1);
	}


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

	char buffer[4096];
	int32_t bytesRead = connSocket->Receive(buffer, 4096);

	std::string msgReceived(buffer, bytesRead);
	LOG("Received message: %s", msgReceived.c_str());
}