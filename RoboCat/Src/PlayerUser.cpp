#include "RoboCatPCH.h"
#include "PlayerUser.h"
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


PlayerUser::PlayerUser()
{
	playerName = "";
}

PlayerUser::PlayerUser(int _pnum, std::string username)
{
	sendSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	recvSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

	playerNumber = _pnum;
	playerName = username;
	//playerName = _pnum == 0 ? "Server" : "Player" + std::to_string(playerNumber);
	//std::cout << playerName << " client "<< " Subsystems Initialized!\n";
}

PlayerUser::~PlayerUser()
{
	closeSockets();
	chatThread.detach();
}

void PlayerUser::startTcpThread(bool isServer)
{
	CLIENT_SEND_PORT = std::to_string(rand() % 1000 + 8999);
	CLIENT_RECV_PORT = std::to_string(rand() % 1000 + 8999);
	if (isServer)
	{
		chatThread = std::thread([this] { initTcpServer(CLIENT_RECV_PORT); });
	}
	else
	{
		chatThread = std::thread([this] { initTcpClient(CLIENT_SEND_PORT, CLIENT_RECV_PORT); });
	}
}

void PlayerUser::initTcpClient(std::string sendPort, std::string recvPort)
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

	std::string address = StringUtils::Sprintf("127.0.0.1:%s", sendPort.c_str());
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

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:"+recvPort);
	if (servAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	if (sendSocket->Connect(*servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}

	LOG("%s", "Connected to server!");

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
			//LOG("Received message: %s", receivedMsg.c_str());
			if (receivedMsg[0] == '$') // SEND MESSAGE
			{
				decodeMessageString(receivedMsg); // display message
			}
			else if (receivedMsg[0] == '~') // CHANGE USERNAME
			{
				playerName = receivedMsg.erase(0, 1);
			}
		}
		});

	//std::cout << "Press enter to exit at any time!\n";
	std::cin.get();
	quit = true;
//	recvConnSocket->~TCPSocket(); // Forcibly close socket (shouldn't call destructors like this -- make a new function for it!
	receiveNewThread.join();
}

void PlayerUser::initTcpServer(std::string listenPort)
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

	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:"+ listenPort);
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

	LOG("Accepted connection from %s", incomingAddress.ToString().c_str());

	quit = false;
	std::thread receiveThread([&]() { // don't use [&] :)
		while (!quit) // Need to add a quit here to have it really exit!
		{
			char buffer[4096];
			int32_t bytesReceived = recvConnSocket->Receive(buffer, 4096);
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
			LOG("Received message from %s: %s", incomingAddress.ToString().c_str(), receivedMsg.c_str());
			if (receivedMsg[0] == '$') // SEND MESSAGE
			{
				decodeMessageString(receivedMsg); // display message
			}
			else if (receivedMsg[0] == '~') // CHANGE USERNAME
			{
				playerName = receivedMsg.erase(0, 1);
			}
		}
		});

	//std::cout << "Press enter to exit at any time!\n";
	std::cin.get();
	quit = true;
//	recvConnSocket->~TCPSocket(); // Forcibly close socket (shouldn't call destructors like this -- make a new function for it!
	//std::cout << "HERE!";
	receiveThread.join();
}

std::string PlayerUser::messageIntoString(std::string _message)
{
	std::string toReturn = "$";
	
	toReturn +=
			playerName   + SEPERATOR_TOKEN +
			_message;

	return toReturn;
}

void PlayerUser::decodeMessageString(std::string _messageString)
{
	if (_messageString.length() == 0)
		return;

	vector<std::string> splitMessageString;

	if (_messageString[0] == '$')
		_messageString = _messageString.erase(0, 1);
	splitMessageString = split(_messageString, SEPERATOR_TOKEN);

	std::string str = "< " + splitMessageString[0] + " >:" + splitMessageString[1] + "\n";

	std::cout << str;
}


void PlayerUser::sendMessage(std::string _message)
{
	if (sendRecvFlag == 0)
	{
		std::string msg = messageIntoString(_message);
		sendSocket->Send(msg.c_str(), msg.length());
	}
	else if (sendRecvFlag == 1)
	{
		std::string msg = messageIntoString(_message);
		recvConnSocket->Send(msg.c_str(), msg.length());
	}
}

void PlayerUser::closeSockets()
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

void PlayerUser::shutdown()
{
	quit = true;
}
