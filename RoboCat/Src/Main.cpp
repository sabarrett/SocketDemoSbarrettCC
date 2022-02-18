
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>

void DoTcpServer();
void DoTcpClient();

void sendMessage(TCPSocketPtr socket, string message); 
string receiveMessage(TCPSocketPtr socket);

TCPSocketPtr establishTCPSocketUsingIPV4(string address, int port); //Create a TCPSocket and Bind it to a given address and port
void connectTCPSocketUsingIPV4(TCPSocketPtr socket, string address, int port);
TCPSocketPtr listenAndAcceptTCPConnection(TCPSocketPtr socket);

bool AOLSend(TCPSocketPtr socket, string username, OutputWindow win);
bool AOLReceive(TCPSocketPtr socket, OutputWindow win);
void AOLReceiveLoop(TCPSocketPtr socket, OutputWindow win);

void write(OutputWindow win, string msg);


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
	system("cls");


	OutputWindow win;

	write(win, "Welcome to the AOL!");
	cout << "Please enter your username: " << endl;
	
	string username, msg;

	cin >> username;


	// Open a TCP socket
	TCPSocketPtr listenSocket = establishTCPSocketUsingIPV4("0.0.0.0", 8080);
	

	write(win, "Bound socket");
	write(win, "Please type /exit to quit!");

	// Call "Listen" to have the OS listen for connections on that socket
	
	TCPSocketPtr conn = listenAndAcceptTCPConnection(listenSocket);

	sendMessage(conn, username + " has joined the channel!");

	cin.ignore(); 

	thread recThread(AOLReceiveLoop, conn, win);

	bool shouldContinue = true;

	while (shouldContinue)
	{
		shouldContinue = AOLSend(conn, username, win);
	}
	
	recThread.join();

	/*std::string msgReceived = receiveMessage(conn);
	LOG("Received message: %s", msgReceived.c_str());

	std::string msg("Hi Client! What a swell day!");
	sendMessage(conn, msg);*/
}

void DoTcpClient()
{
	system("cls");


	OutputWindow win;

	write(win, "Welcome to the AOL!");
	cout << "Please enter your username: " << endl;

	string username, msg;

	cin >> username;
	
	TCPSocketPtr connSocket = establishTCPSocketUsingIPV4("127.0.0.1", 8085);

	connectTCPSocketUsingIPV4(connSocket, "127.0.0.1", 8080);

	write(win, "Connected to server!");
	write(win, "Please type /exit to quit!");

	sendMessage(connSocket, username + " has joined the channel!");

	cin.ignore();

	thread recThread(AOLReceiveLoop, connSocket, win);

	bool shouldContinue = true;

	while (shouldContinue)
	{
		shouldContinue = AOLSend(connSocket, username, win);
	}
	
	recThread.join();

	/*string msg("Hello, server! How are you today?");
	sendMessage(connSocket, msg);

	LOG("%s", "Sent message to server");

	string msgReceived = receiveMessage(connSocket);
	
	LOG("Received message: %s", msgReceived.c_str());*/
}

void sendMessage(TCPSocketPtr socket, string message)
{
	socket->Send(message.c_str(), message.length());
}

string receiveMessage(TCPSocketPtr socket)
{
	char buffer[4096];
	int32_t bytesRead = socket->Receive(buffer, 4096);

	string msgReceived(buffer, bytesRead);

	return msgReceived;
}

TCPSocketPtr establishTCPSocketUsingIPV4(string address, int port)
{
	// Open a TCP socket
	TCPSocketPtr connSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (connSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listen socket");
		ExitProcess(1);
	}


	//SocketAddress a2(INADDR_LOOPBACK, 8080);
	// Listen only for connections on this machine

	string myAddress = address + ":" + to_string(port);

	SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString(myAddress);
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

	return connSocket;
}

void connectTCPSocketUsingIPV4(TCPSocketPtr socket, string address, int port)
{
	string connAddress = address + ":" + to_string(port);
	SocketAddressPtr b = SocketAddressFactory::CreateIPv4FromString(connAddress);
	if (b == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	if (socket->Connect(*b) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}
}

TCPSocketPtr listenAndAcceptTCPConnection(TCPSocketPtr socket)
{
	if (socket->Listen() != NO_ERROR)
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

	conn = socket->Accept(connAddr);

	return conn;
}

bool AOLSend(TCPSocketPtr socket, string username, OutputWindow win)
{
	string msg;
	cout << username << ": ";
	getline(cin, msg);
	write(win, username + ": " + msg);
	win.Scroll(-1);


	if (msg == "/exit")
	{
		sendMessage(socket, "/exit");
		return false;
	}

	sendMessage(socket, username + ": " + msg);
	return true;
}

bool AOLReceive(TCPSocketPtr socket, OutputWindow win)
{
	string msg;
	msg = receiveMessage(socket);

	if (msg == "/exit")
	{
		return false;
	}

	write(win, msg);
	return true;
}

void AOLReceiveLoop(TCPSocketPtr socket, OutputWindow win)
{
	bool shouldContinue = true;
	while (shouldContinue)
	{
		shouldContinue = AOLReceive(socket, win);
	}
}

void write(OutputWindow win, string msg)
{
	win.Write(msg);
	win.Scroll(-1);
}