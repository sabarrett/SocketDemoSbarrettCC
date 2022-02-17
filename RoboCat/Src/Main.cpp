
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>


void BradsTotallyOriginalServer()
{
	// create a socket to listen for input data
	TCPSocketPtr listeningSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	// check if the socket was created, if not kill the program
	if (listeningSocket == nullptr)
	{
		SocketUtil::ReportError("Could not create socket: Null Ptr");
		ExitProcess(1); // kill
	}
	LOG("%s", "Listening socket created");

	//see if this will get the port from the command line
	//std::string port = StringUtils::GetCommandLineArg(1);
	//LOG("%s", "WOWZA! You just told me: " + port)
	// 
	//create a prt for the port
	SocketAddressPtr addressPtr = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:8080"); // a hard-coded port to bind to
	if (addressPtr == nullptr)
	{
		SocketUtil::ReportError("Could not create server address");
		ExitProcess(1); // kill
	}

	LOG("%s", "WOW! You made an address for your socket");

	//Bind Time!
	bool bindError = listeningSocket->Bind(*addressPtr) != NO_ERROR;
	if (bindError)
	{
		SocketUtil::ReportError("Socket could not bind to address");
		ExitProcess(1); // kill
	}

	LOG("%s", "The binding is complete");

	bool listenError = listeningSocket->Listen() != NO_ERROR;
	if (listenError)
	{
		SocketUtil::ReportError("Listen Error");
		ExitProcess(1); // kill
	}

	LOG("%s", "We're listening for incoming connections");

	// accept is a blocking function so it will halt the program execution until it recieves a incoming connection

	LOG("%s", "Waiting to accept connections.oof.oof.oof");

	SocketAddress incomingAddress;
	TCPSocketPtr incomingSocket = listeningSocket->Accept(incomingAddress);
	while (incomingSocket == nullptr) // if it didnt work the first time, try again
	{
		incomingSocket = listeningSocket->Accept(incomingAddress);
	}
	
	// CONECTION ACCEPTED WE HAVE CONTACT ==============================
	LOG("Bleep Bloop...Accepted connection from: %s", incomingAddress.ToString().c_str());

	//attempt to connect back to the client
	/*
	if (incomingSocket->Connect(incomingAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}
	*/

	//send data
	std::thread t1([&]()
		{
			std::string msg("");
			while (msg != "/exit")
			{
				std::getline(std::cin, msg);
				incomingSocket->Send(msg.c_str(), msg.length());
			}
		});

	//receive data
	char buffer[4096];
	int32_t bytesReceived = incomingSocket->Receive(buffer, 4096);
	while (bytesReceived < 4096)
	{
		bytesReceived = incomingSocket->Receive(buffer, 4096);
		std::string receivedMsg(buffer, bytesReceived);
		std::cout << receivedMsg << std::endl;
		//LOG("%s: %s", incomingAddress.ToString().c_str(), receivedMsg.c_str());		
	}
}

void BradsLessOriginalClient()
{
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
	{
		SocketUtil::ReportError("Could not create client socket");
		ExitProcess(1);
	}

	LOG("%s", "Client socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address
	std::string port;
	port = StringUtils::GetCommandLineArg(2);
	std::string address = StringUtils::Sprintf("127.0.0.1:%s", port.c_str());
	//std::string address = StringUtils::Sprintf("127.0.0.1:8081");
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError("Creating client address");
		ExitProcess(1);
	}

	if (clientSocket->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding client socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound client socket");

	// Connect() -> Connect socket to remote host

	SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
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
	//CONECTION CREATED WE HAVE CONTACT ==============================
	std::string msg("Hello You are connected!");
	clientSocket->Send(msg.c_str(), msg.length());

	std::thread t([&]()
		{
			char buffer[4096];
			int32_t bytesReceived = clientSocket->Receive(buffer, 4096);
			while (bytesReceived < 4096)
			{
				bytesReceived = clientSocket->Receive(buffer, 4096);
				std::string receivedMsg(buffer, bytesReceived);
				std::cout << receivedMsg << std::endl;
			}
		});

	//OutputWindow win;

	msg = "";
	while (msg != "/exit")
	{
		std::getline(std::cin, msg);
		clientSocket->Send(msg.c_str(), msg.length());
		//std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
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
	bool isServer = StringUtils::GetCommandLineArg(1) == "server"; // check if the command on the executable is 'server'
	if (isServer) // if it is the server
	{
		BradsTotallyOriginalServer();	
	}

	// Client Code --------------
	if (!isServer)
	{
		BradsLessOriginalClient();
	}

	SocketUtil::CleanUp();

	return 0;
}

// getInput(); (but don't block!)
//		check sockets -- did anyone say anything?
//			conn->Receive(); !!!!!! <- blocks!!!
//		Solution: non-blocking mode
//			if (conn->Receive() == -10035)
//				skip this frame; Receive would block
// 
// main
// |   spawnThread() -------->	netThread
// |   getInput();				| block on Accept(); ....
// |   update();				| spawnThread() ----------->	connThread
// |   goto beginning;			| loop();						| conn->Receive(); (blocks)
// |							|								|
// |
// v
// 
// Select();
// 
// update();
// render();
// repeat();

/*
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
*/
