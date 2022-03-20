
#include "RoboCatPCH.h"
#include "InputSystem.h"
#include "GraphicsLibrary.h"
#include "Colour.h"

//Some functions to make shapes for the game objects to track and send info about
//al_draw_circle
//al_draw_triangle
//al_draw_rectangle

#if _WIN32

void DoTCPServer()
{
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("creating listening socket");
		ExitProcess(1);
	}

	LOG("%s", "created listening socket");

	SocketAddress a(INADDR_ANY, 8080);

	if (&a == nullptr)
	{
		SocketUtil::ReportError("creating server address");
		ExitProcess(1);
	}

	LOG("%s", "created socket address");

	if (listenSocket->Bind(a) != NO_ERROR)
	{
		SocketUtil::ReportError("binding socket");
		ExitProcess(1);
	}

	LOG("%s", "bound socket");

	// Call "Listen" to have the OS listen for connections on that socket
	if (listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("listening");
		ExitProcess(1);
	}

	LOG("%s", "socket listening");

	SocketAddress connAddr(INADDR_LOOPBACK, 8081);
	TCPSocketPtr conn;

	// This code isn't blocking anymore -- it'll run to the end of the program.
	//while (conn != nullptr)
	{
		conn = listenSocket->Accept(connAddr);
	}

	bool messageOnGoing = true;
	//messageOnGoing = TCPServerSendMessages(conn, connAddr);

	while (messageOnGoing == true)
	{
		//messageOnGoing = TCPServerSendMessages(conn, connAddr);
	}
}

void DoTCPClient()
{
	TCPSocketPtr connSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

	if (connSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listen socket");
		ExitProcess(1);
	}

	LOG("%s", "Created client socket");

	//SocketAddress a2(INADDR_LOOPBACK, 8080);
	// Listen only for connections on this machine
	//SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString(INADDR_ANY);
	SocketAddress a(INADDR_LOOPBACK, 8081);

	if (&a == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	LOG("%s", "Create socket address");

	// "Bind" that socket to the address we want to listen on
	if (connSocket->Bind(a) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding socket");
		ExitProcess(1);
	}

	LOG("%s", "Bound socket");

	//SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString(INADDR_ANY);
	SocketAddress servAddress(INADDR_LOOPBACK, 8080);

	if (&servAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	if (connSocket->Connect(servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}

	LOG("%s", "Connected to server!");
	std::string msg("someone has connected");
	connSocket->Send(msg.c_str(), msg.length());

	bool messageOnGoing = true;
	//messageOnGoing = TCPClientSendMessages(connSocket, servAddress);


	while (messageOnGoing == true)
	{
		//messageOnGoing = TCPClientSendMessages(connSocket, servAddress);
	}
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

	//while (activeConnection == false)
	InputSystem mInputSystem; 
	bool activeConnection; // = true

	//something to start up the connection (info from assignment 1?)
	//then add the keys for making the objects spawn (3 objects, 3 keys)
		//need something to track the objects made? idk if there is something i can use?
	//each update send out the information about the object type and its position
		//then prepare to receive it as well?
	//if (mInputSystem.getKeyboardInput())
	
	
	SocketUtil::CleanUp();

	return 0;
}