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

std::string username;
bool quitApp = false;
std::pair<SocketAddress, std::string> chatPartnerInfo;
OutputWindow win;
bool typing;
int linesPrinted = 0;

void enterChat()
{
	std::cout << "Username: ";
	std::getline(std::cin, username);
	if (username == "")
		username = "Put addr here";
	//win.Write(username + "has joined the chat\n\n");
	std::cout << username << " has joined the chat\n\n";
	linesPrinted += 4;
}



void DoTcpServer()
{
	// Open a TCP socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listen socket");
		ExitProcess(1);
	}

	//LOG("%s", "Created listen socket");
	//std::cout << ".";
	win.WriteFromStdin(".");


	//SocketAddress a2(INADDR_LOOPBACK, 8080);
	// Listen only for connections on this machine
	SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080"); //will change to 0.0.0.0
	if (a == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	//LOG("%s", "Create socket address");
	win.WriteFromStdin("..");

	// "Bind" that socket to the address we want to listen on
	if (listenSocket->Bind(*a) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding socket");
		ExitProcess(1);
	}

	//LOG("%s", "Bound socket");
	win.WriteFromStdin("...");

	// Call "Listen" to have the OS listen for connections on that socket
	if (listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening");
		ExitProcess(1);
	}

	//LOG("%s", "Socket listening");
	win.WriteFromStdin("----------Ready to Connect----------");

	// Call "Accept" which *blocks* until we get a request to connect,
	// and then it accepts that connection!


	/// listenSocket <----- packets from other hosts wanting to connect
	/// listenSocket -----> accepts connection, spawns:
	///   connectionSocket <----> otherHost1
	///   connectionSock2  <----> otherHost2

	SocketAddress connAddr;
	TCPSocketPtr conn;


	while (conn == nullptr)
	{
		conn = listenSocket->Accept(connAddr);
	}

	enterChat();

	// This code isn't blocking anymore -- it'll run to the end of the program.
	
	conn->Send(username.c_str(), username.length());
	
	char userBuffer[4096];
	int32_t userBytes = conn->Receive(userBuffer, 4096);

	std::string partnerUserName(userBuffer, userBytes);
	if (userBytes <= 0)
	{
		partnerUserName = connAddr.ToString().c_str();
	}

	chatPartnerInfo.first = connAddr;
	chatPartnerInfo.second = partnerUserName;

	LOG("Connected to %s", chatPartnerInfo.second.c_str(), "\n\n\n\n\n\n\n\n\n\n");

	/*LOG("%s: ", username.c_str());*/
	std::cout << username.c_str() << ":oo ";

	std::thread tSend([&conn]()
	{
		while (!quitApp)
		{

			std::string sentMsg;
			std::getline(std::cin, sentMsg);
			if (sentMsg == "/quit")
			{
				quitApp = true;
				//conn->~TCPSocket();
				conn = nullptr;
				break;
			}
			else
			{
				conn->Send(sentMsg.c_str(), sentMsg.length());
				//LOG("%s: ", username.c_str());
				std::cout << username.c_str() << ":// ";
			}
		}

		return;

	});

	std::thread tRecieve([&conn]()
	{
		while (!quitApp)
		{
			char buffer[4096];
			int32_t bytesRead = conn->Receive(buffer, 4096);

			
			//win.WriteFromStdin(" ");
			if (bytesRead <= 0)
			{
				LOG("%s has disconnected. ", chatPartnerInfo.second.c_str());
				conn->~TCPSocket();
				conn = nullptr;
				break;
			}
			std::string msgReceived(buffer, bytesRead);
			//std::cout << " ";
			win.MoveBackwards();
			win.WriteFromStdin("REWRITE");
			LOG("%s: %s", chatPartnerInfo.second.c_str(), msgReceived.c_str());
			//LOG("%s: ", username.c_str());
			std::cout << username.c_str() << ":~ ";
		}

		return;
	});

	//std::thread tTyping([&conn]()
	//{
	//	while (!quitApp)
	//	{
	//		std::cout << "Trying\n";

	//		//conn->Send(typing, typing.length());
	//		conn->Send("true", 1);

	//		bool buffer[1];
	//		int32_t bytesRead = conn->Receive(buffer, 1);

	//		if (bytesRead == 0)
	//			LOG("%s is typing...\n", chatPartnerInfo.second.c_str());

	//		if (bytesRead == 1)
	//		{
	//			std::cout << "Here\n";
	//			win.ClearLine(1);// Write(username + "has joined the chat\n\n");
	//		}
	//	}
	//	return;
	//});


	tSend.join();
	tRecieve.join();
	//tTyping.join();
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

	enterChat();

	connSocket->Send(username.c_str(), username.length());

	char userBuffer[4096];
	int32_t userBytes = connSocket->Receive(userBuffer, 4096);

	std::string partnerUserName(userBuffer, userBytes);
	if (userBytes <= 0)
	{
		//partnerUserName = connSocket.ToString().c_str();
		//SocketUtil::ReportError("Recieving user");
		//return;
	}

	chatPartnerInfo.first = *servAddress;
	chatPartnerInfo.second = partnerUserName;

	LOG("Connected to %s", chatPartnerInfo.second.c_str());

	std::thread tSend([&connSocket]()
	{
		while (!quitApp)
		{
			std::string sentMsg;
			std::getline(std::cin, sentMsg);
			if (sentMsg == "/quit")
			{
				quitApp = true;
				//connSocket->~TCPSocket();
				//connSocket->~TCPSocket();
				connSocket = nullptr;
				break;
			}
			else
			{
				connSocket->Send(sentMsg.c_str(), sentMsg.length());
			}
		}
		return;

	});

	std::thread tRecieve([&connSocket]()
	{
		while (!quitApp)
		{
			char buffer[4096];
			int32_t bytesRead = connSocket->Receive(buffer, 4096);

			if (bytesRead <= 0)
			{
				LOG("%s has disconnected. ", chatPartnerInfo.second.c_str());
				connSocket->~TCPSocket();
				connSocket = nullptr;
				break;
			}
			std::string msgReceived(buffer, bytesRead);
			LOG("%s: %s", chatPartnerInfo.second.c_str(), msgReceived.c_str());
		}
		return;
	});

	tSend.join();
	tRecieve.join();
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

	//std::thread t();
//t.join();
	OutputWindow win;



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

	/*std::thread tWin([&win]()
	{
		while (!quitApp)
		{
			win.MoveCursorToScreenBottom();
		}
		return;
	});*/

	SocketUtil::CleanUp();


	system("pause");
	return 0;
}