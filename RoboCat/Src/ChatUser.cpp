#include "RoboCatPCH.h"
#include "ChatUser.h"
#include <iostream>
#include <thread>

// FUNCTION FROM user704565 AT https://stackoverflow.com/a/16286297:
std::vector<std::string> split(std::string str, std::string sep) {
	char* cstr = const_cast<char*>(str.c_str());
	char* current;
	std::vector<std::string> arr;
	current = strtok(cstr, sep.c_str());
	while (current != NULL) {
		arr.push_back(current);
		current = strtok(NULL, sep.c_str());
	}
	return arr;
}

ChatUser::ChatUser()
{
	SocketUtil::StaticInit();
	sendSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	recvSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

}

ChatUser::~ChatUser()
{
	send("©DISCONNECT");
	shutdown();
	closeSockets();
	SocketUtil::CleanUp();
}

void ChatUser::startTcpThread(bool isClient)
{
	username = isClient ? "Chat User 1" : "Chat User 2";

	if (isClient)
		t = std::thread([this] { initTcpClient(); });
	else
		t = std::thread([this] { initTcpServer(); });
}

void ChatUser::initTcpClient()
{
	sendRecvFlag = 0;

	// Create socket
	sendSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (sendSocket == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
	}

	LOG("%s", "Client socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	std::string address = StringUtils::Sprintf("127.0.0.1:%s", SEND_PORT.c_str());
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError("Creating client address");
		ExitProcess(1);
	}

	if (sendSocket->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding client socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound client socket");

	// Connect() -> Connect socket to remote host

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:"+RECV_PORT);
	if (servAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	if (sendSocket->Connect(*servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}

	LOG("%s", "Connected to server!");
	hasConnected = true;
	send("©CONNECT");
	quit = false;
	std::thread receiveNewThread([&]() { // don't use [&] :)
		while (!quit) // Need to add a quit here to have it really exit!
		{
			char buffer[4096];
			int32_t bytesReceived = sendSocket->Receive(buffer, 4096);
			if (bytesReceived == 0)
			{
				// handle disconnect
			}
			if (bytesReceived < 0)
			{
				SocketUtil::ReportError("Receiving");
				return;
			}

			std::string receivedMsg(buffer, bytesReceived);
			recv(receivedMsg);
		}
		});
	receiveNewThread.join();
}

void ChatUser::initTcpServer()
{
	sendRecvFlag = 1;
	// Create socket
	recvSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (recvSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}

	//recvSocket->SetNonBlockingMode(true);

	LOG("%s", "Listening socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:"+ RECV_PORT);
	if (listenAddress == nullptr)
	{
		SocketUtil::ReportError("Creating listen address");
		ExitProcess(1);
	}

	if (recvSocket->Bind(*listenAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding listening socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound listening socket");

	// Blocking function call -> Waits for some input; halts the program until something "interesting" happens
	// Non-Blocking function call -> Returns right away, as soon as the action is completed

	// Listen() - Listen on socket -> Non-blocking; tells OS we care about incoming connections on this socket
	if (recvSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on listening socket");
		ExitProcess(1);
	}

	LOG("%s", "Listening on socket");

	// Accept() - Accept on socket -> Blocking; Waits for incoming connection and completes TCP handshake

	LOG("%s", "Waiting to accept connections...");
	SocketAddress incomingAddress;
	recvConnSocket = recvSocket->Accept(incomingAddress);
	while (recvConnSocket == nullptr)
	{
		recvConnSocket = recvSocket->Accept(incomingAddress);
		// SocketUtil::ReportError("Accepting connection");
		// ExitProcess(1);
	}
	hasConnected = true;
	send("©CONNECT");
	LOG("Accepted connection from %s", incomingAddress.ToString().c_str());

	quit = false;
	std::thread receiveThread([&]() { // don't use [&] :)
		while (!quit) // Need to add a quit here to have it really exit!
		{
			char buffer[4096];
			int32_t bytesReceived = recvConnSocket->Receive(buffer, 4096);
			if (bytesReceived == 0)
			{
				// handle disconnec
			}
			if (bytesReceived < 0)
			{
				SocketUtil::ReportError("Receiving");
				return;
			}

			std::string receivedMsg(buffer, bytesReceived);
			recv(receivedMsg);
		}
		});

	receiveThread.join();
}

void ChatUser::send(std::string msg)
{
	if (strlen(msg.c_str()) > 0)
	{
		std::string toSend = username + "¦" + msg;
		if (msg == "©DISCONNECT")
			toSend = "©[SERVER]: " + username + " has disconnected!\n";
		if (msg == "©CONNECT")
			toSend = "©[SERVER]: " + username + " has connected!\n";

		(sendRecvFlag == 0 ? sendSocket : recvConnSocket)->Send(toSend.c_str(), toSend.length());
	}
}

void ChatUser::recv(std::string msg)
{
	if (msg[0] == '©')
	{
		std::cout << msg.erase(0, 1);
	}
	else
	{
		std::vector<std::string> str = split(msg, "¦");
		std::cout << ("[" + str[0] + "]: " + str[1] + "\n");
	}
}

void ChatUser::closeSockets()
{
	if (sendRecvFlag == 0)
	{
		sendSocket->~TCPSocket();
	}
	else if (sendRecvFlag == 1)
	{
		
		recvConnSocket->~TCPSocket();
		recvConnSocket->~TCPSocket();
	}
}

void ChatUser::shutdown()
{
	quit = true;
}
