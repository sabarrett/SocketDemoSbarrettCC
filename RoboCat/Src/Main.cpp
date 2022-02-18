
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>

enum class PacketType {
	UNKNOWN,
	HELLO,
	LEAVE,
	MESSAGE
};

struct PacketHeader {
	PacketType type;
};

struct User {
	SocketAddress inAddr;
	SocketAddress outAddr;
	std::string username;
};




std::vector<User> currentUsers;

bool FindUserFromAddr(SocketAddress addr, User& out) {
	for (User& user : currentUsers) {
		if (user.inAddr == addr) {
			out = user;
			return true;
		}
	}
	return false;
}

void ProcessInput(std::string input);


OutputWindow win(&ProcessInput);

void HandleMessage(std::string message);
void HandleCommand(std::string cmd_full);
void ProcessInput(std::string input) {

	if (input[0] == '/') {
		HandleCommand(input);
	}
	else {
		HandleMessage(input);
	}
}

void SendPacket(SocketAddress remoteAddress, PacketType type, std::string content);

void HandleMessage(std::string message) {
	win.Write(message);

	for (User& user : currentUsers) {

		SendPacket(user.outAddr, PacketType::MESSAGE, message);

	}
}


SocketAddressPtr outAddr;
SocketAddressPtr g_inAddr;

TCPSocketPtr inSocket;
bool isListening;
std::string username;

void ListenLoop();
void StartSocket(SocketAddressPtr inAddress) {
	inSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	g_inAddr = inAddress;

	if (inSocket == nullptr) {
		SocketUtil::ReportError("Creating sockets");
	}


	if (inSocket->Bind(*g_inAddr) != NO_ERROR) {
		SocketUtil::ReportError("Binding listening socket");
		return;
	}

	if (inSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on listening socket");
		return;
	}

	win.Write("server started", FOREGROUND_GREEN);

	isListening = true;

	std::thread listenThread(ListenLoop);

	listenThread.detach();

}

TCPSocketPtr ConnectSocket(SocketAddress remoteAddress) {
	TCPSocketPtr outSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

	if (inSocket == nullptr) {
		SocketUtil::ReportError("Creating sockets");
		return nullptr;
	}

	if (outSocket->Bind(*outAddr) != NO_ERROR) {
		SocketUtil::ReportError("Binding listening socket");
		return nullptr;
	}
	if (outSocket->Connect(remoteAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		return nullptr;
	}

	return outSocket;
}

void SendPacket(SocketAddress remoteAddress, PacketType type, std::string content) {
	char buffer[4096];
	TCPSocketPtr outSocket = ConnectSocket(remoteAddress);

	PacketHeader* hdr = (PacketHeader*)buffer;
	char* content_buf = (char*)(hdr + 1);

	hdr->type =type;
	memcpy(content_buf, content.c_str(), content.length());

	outSocket->Send(buffer, content.length() + sizeof(PacketHeader));

	outSocket->Shutdown();
}

void HelloClient(SocketAddressPtr remoteAddress) {
	SendPacket(*remoteAddress, PacketType::HELLO, g_inAddr->ToString());
}

void ListenLoop() {

	while (isListening)
	{

		SocketAddress incomingAddress;
		TCPSocketPtr socket = inSocket->Accept(incomingAddress);


		if (socket != nullptr)
		{
			char buffer[4096];
			int32_t bytesReceived = socket->Receive(buffer, 4096);
			if (bytesReceived > 0) {
				if (bytesReceived >= sizeof(PacketHeader))
				{
					PacketHeader hdr = *(PacketHeader*)buffer;
					std::string content(buffer + sizeof(PacketHeader), bytesReceived - sizeof(PacketHeader));
					if (hdr.type == PacketType::HELLO) {
						SocketAddressPtr outAddr = SocketAddressFactory::CreateIPv4FromString(content);
						User user;
						if (!FindUserFromAddr(incomingAddress, user)) {
							currentUsers.push_back(User{ incomingAddress, *outAddr, content });
							//HelloClient(outAddr);
							win.Write("user " + content + " joined", FOREGROUND_BLUE);
						}
					}
					if (hdr.type == PacketType::MESSAGE) {
						User sender;
						bool found = FindUserFromAddr(incomingAddress, sender);
						if (found) {
							win.Write(sender.username + ": " + content);
						}
					}
				}
				else {
					win.Write("recived invalid packet", FOREGROUND_RED);
				}
			}
		}
	}

	inSocket->~TCPSocket();
}


void HandleCommand(std::string cmd_full) {

	// split command by spaces
	std::vector<std::string> cmd_parts;
	std::size_t start = 0;
	for (std::size_t i = 0; i < cmd_full.size(); i++) {
		if (cmd_full[i] == ' ') {
			cmd_parts.push_back(cmd_full.substr(start, i - start));
			start = i + 1;
			win.Write(cmd_parts[cmd_parts.size() - 1]);
		}
	}
	cmd_parts.push_back(cmd_full.substr(start, cmd_full.size() - start));

	if (cmd_parts[0] == "/start") {
		if (cmd_parts.size() < 2) {
			win.Write("missing port number, try using /help", FOREGROUND_RED);
			return;
		}
		SocketAddressPtr inAddr = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:" + cmd_parts[1]);
		
		if (!inAddr)
			win.Write("unable to parse port, try /help", FOREGROUND_RED);

		win.Write("starting server on " + cmd_parts[1], FOREGROUND_GREEN);
		StartSocket(inAddr);
	}
	else if (cmd_parts[0] == "/set_out") {
		if (cmd_parts.size() < 2) {
			win.Write("missing port number, try using /help", FOREGROUND_RED);
			return;
		}
		SocketAddressPtr tempAddr = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:" + cmd_parts[1]);

		if (!tempAddr) {
			win.Write("unable to parse port, try /help", FOREGROUND_RED);
			return;
		}

		outAddr = tempAddr;
	}
	else if (cmd_parts[0] == "/set_username") {
		if (cmd_parts.size() < 2) {
			win.Write("missing username", FOREGROUND_RED);
		}

		username = cmd_parts[1];
		win.Write("set username to " + username);
	}
	else if (cmd_parts[0] == "/hello") {
		if (cmd_parts.size() < 2) {
			win.Write("missing ip, try using /help", FOREGROUND_RED);
			return;
		}

		if (username.size() == 0) {
			win.Write("missing username, try using /help", FOREGROUND_RED);
			return;
		}
		SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString(cmd_parts[1]);
		if (!listenAddress)
			win.Write("unable to address, try /help", FOREGROUND_RED);

		win.Write("joinging on " + cmd_parts[1], FOREGROUND_GREEN);

		HelloClient(listenAddress);
	}
	else if (cmd_parts[0] == "/help") {
		win.Write("/start [inport] [outport]");
		win.Write("/hello [ip]:[port] [username]");
	} else {
		win.Write("unknown command: \"" + cmd_full + "\"", FOREGROUND_RED);
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

	SocketUtil::StaticInit();



	
	std::thread t([]()
				  {
					  int msgNo = 1;
					  while (true)
					  {
						  std::this_thread::sleep_for(std::chrono::milliseconds(250));
						  std::string msgIn("~~~auto message~~~");
						  std::stringstream ss(msgIn);
						  ss << msgNo;
						  //win.Write(ss.str());
						  msgNo++;
					  }
				  });
	

	CHAR_INFO charInfo[5000] = {};
	int char_count = 0;
	int i = 0;
	while (true)
	{
		
		win.HandleEvents();
		win.Draw();
	
	}

	SocketUtil::CleanUp();

	return 0;
}
