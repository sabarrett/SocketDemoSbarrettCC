
#include "RoboCatPCH.h"

#if _WIN32
void DoTcpServer()
{
	// Open a TCP Socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listen socket");
		ExitProcess(1);
	}
	LOG("%s", "Created Listen Socket");

	//Create Address

	//Listen only for connections on this machine: 127.0.0.1
	SocketAddressPtr addy = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	if (addy == nullptr)
	{
		SocketUtil::ReportError("Creating Server Address");
		ExitProcess(1);
	}
	LOG("%s", "Created Socket Address");

	// "Bind" that socket to what address we want to listen on
	if (listenSocket->Bind(*addy) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding Socket");
		ExitProcess(1);
	}

	LOG("%s", "Bound Socket");

	// call "Listen" to have the os listen for connections on that socket
	if (listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening");
		ExitProcess(1);
	}

	LOG("%s", "Listening");
	// call "Accept" which *Blocks* until we get a request to connect
	// and then it accepts that connection

	//Listen Socket <- packets from other hosts wanting to connect
	//Listen Socket -> accepts connection, spawns:
	//	connectionSocket <--> otherHost1

	SocketAddress connAddy;
	TCPSocketPtr conn = listenSocket->Accept(connAddy);
	LOG("%s", "Accepting Connections");

	char buffer[4096];
	int32_t bytesRead = conn->Receive(buffer, 4096);

	std::string msgRecevied(buffer, bytesRead);

	LOG("Recevied Message: %s", msgRecevied.c_str());
}

void DoTcpClient()
{
	// Open a TCP Socket
	TCPSocketPtr connSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (connSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listen socket");
		ExitProcess(1);
	}
	LOG("%s", "Created Listen Socket");

	//Create Address

	//Listen only for connections on this machine: 127.0.0.1
	SocketAddressPtr addy = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8081");
	if (addy == nullptr)
	{
		SocketUtil::ReportError("Creating Server Address");
		ExitProcess(1);
	}
	LOG("%s", "Created Socket Address");

	// "Bind" that socket to what address we want to listen on
	if (connSocket->Bind(*addy) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding Socket");
		ExitProcess(1);
	}

	LOG("%s", "Bound Socket");

	SocketAddressPtr servAddy = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	if (servAddy == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	if (connSocket->Connect(*servAddy) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting Socket");
		ExitProcess(1);
	}

	LOG("%s", "Connected to server");

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
	

	//Winsock2.h

	SocketUtil::StaticInit();

	
	bool isServer = StringUtils::GetCommandLineArg(1) == "server";
	if (isServer)
	{
		// Server code----------
		DoTcpServer();
	}
	else
	{
		// Client Code----------
		DoTcpClient();
	}

	

	SocketUtil::CleanUp();

	return 0;
}






