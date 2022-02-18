
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <limits>
#include <ios>

std::atomic<bool> isInASession{ false };
std::atomic<bool> isConnectingToAValidPort { false };
std::atomic<bool> isWaitingForOthers { true };
std::atomic<bool> closingConnections { false };
std::atomic<int> nextAvailablePort{ 8080 };

/// <summary>
/// Based from the DoTcpServer() in the in-class demo
/// </summary>
void SetUpInitialListening(int &port, TCPSocketPtr &listeningSocket, SocketAddressPtr &listeningAddress)
{
	//  1
		// here we can select which IPV we're using, and IPV6 is a bit wild for these purposes so, we go with IPV4
	listeningSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listeningSocket == nullptr)
	{
		SocketUtil::ReportError("Error Creating Listen Socket");
		ExitProcess(1);
	}
	LOG("%s","created listening socket");

	nextAvailablePort = port;

	listeningAddress = SocketAddressFactory::CreateIPv4FromString( ("0.0.0.0:" + std::to_string(nextAvailablePort++)) );
	if (listeningAddress == nullptr)
	{
		SocketUtil::ReportError("Error creating server address");
		ExitProcess(1);
	}

	LOG("%s", "binding the socket");
	while (listeningSocket->Bind(*listeningAddress) != NO_ERROR)
	{

		//LOG("%s", "port: 0.0.0.0:" + std::to_string(nextAvailablePort) + " taken, trying to use port: 0.0.0.0:" + std::to_string(nextAvailablePort + 1));
		listeningAddress = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:" + std::to_string(nextAvailablePort));
		++nextAvailablePort;
	}

	LOG("%s", "bound the socket");

	if (listeningSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Error listening with the socket");
		ExitProcess(1);
	}

	LOG("Your port is %i", static_cast<int>(nextAvailablePort - 1));
	LOG("%s", "socket is now listening");
}

void HandleListening(TCPSocketPtr& listeningSocket, std::map<int, TCPSocketPtr> &connectedSockets, queue<std::pair<int, string>>&messages)
{

	SocketAddressPtr connAddr;
	TCPSocketPtr conn = nullptr;

	while (!closingConnections)
	{
		if (conn == nullptr) // check for new people
		{
			LOG("%s", "socket is still listening");
			conn = listeningSocket->Accept(*connAddr);
		}
		else  //process it for the first time
		{
			char buffer[4096];
			int32_t bytesRead = conn->Receive(buffer, 4096);
			std::string msgRecieved(buffer, bytesRead);

			LOG("Established connection with port: %s", msgRecieved.c_str());
			connectedSockets.emplace(std::stoi(msgRecieved), conn);
			conn = nullptr;
		}


		for each (std::pair<int, TCPSocketPtr> curConn in connectedSockets)
		{
			if (curConn.second == nullptr)
			{
				LOG("%s", "LOGGED A CONNECTED SOCKET THAT'S BROKEN");
			}
			else
			{
				char buffer[4096];
				int32_t bytesRead = curConn.second->Receive(buffer, 4096); 

				std::string msgRecieved(buffer, bytesRead);
				LOG("Recieved message: %s", msgRecieved.c_str());
				messages.push(std::pair<int, std::string> (curConn.first, buffer));
			}
		}
	}

	// 
}

void SetUpSending(int portToConnectTo, int listeningPort, std::map<int, TCPSocketPtr> &sendingSockets)
{
	//  1
// here we can select which IPV we're using, and IPV6 is a bit wild for these purposes so, we go with IPV4
	TCPSocketPtr newSendingSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (newSendingSocket == nullptr)
	{
		SocketUtil::ReportError("Error Creating Sending Socket");
		ExitProcess(1);
	}
	LOG("%s", "created connection socket");

	LOG("Your sending port is %i", static_cast<int>(nextAvailablePort));
	SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString( ("127.0.0.1:" + std::to_string(nextAvailablePort++) ));
	//SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:9001");
	if (a == nullptr)
	{
		SocketUtil::ReportError("Error creating sending address");
		ExitProcess(1);
	}
	LOG("%s", "created connection socket address");

	LOG("%s", "binding the connection socket");
	while (newSendingSocket->Bind(*a) != NO_ERROR)
	{
		//LOG("%s", "port: 127.0.0.1:" + std::to_string(nextAvailablePort) + " taken, using port: 127.0.0.1:" + std::to_string(nextAvailablePort));
		a = SocketAddressFactory::CreateIPv4FromString( ("127.0.0.1:" + std::to_string(nextAvailablePort)));
		++nextAvailablePort;
	}
	LOG("%s", "finished binding the connection socket");


	LOG("%s%i", "127.0.0.1:" , portToConnectTo)
	SocketAddressPtr foreignAddress = SocketAddressFactory::CreateIPv4FromString(("127.0.0.1:" + std::to_string(portToConnectTo)));  // this has to match the server's address, and it MUST NOT match client's own
	//SocketAddressPtr foreignAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8000");  // this has to match the server's address, and it MUST NOT match client's own
	if (foreignAddress == nullptr)
	{
		SocketUtil::ReportError("Error	creating foreign listener address");
		ExitProcess(1);
	}

	
	LOG("%s", "Creating server");

	if (newSendingSocket->Connect(*foreignAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Error connecting to server");
		ExitProcess(1);
	}

	LOG("%s", "Connected to server");

	//std::string msg("Hello, server! How are you today?");
	std::string msg(std::to_string(listeningPort));
	newSendingSocket->Send(msg.c_str(), msg.length());

	sendingSockets.emplace(portToConnectTo, newSendingSocket);
}


string GetPortHandle(int port, std::map<int, string>& userHandles)
{
	string result = userHandles.find(port)->second;
	if (userHandles.find(port) == userHandles.end())
		return "";

	return result;
}

string HandleMessages(queue<std::pair<int, string>>& messages, std::map<int, string>& userHandles)
{
	std::string finalLine;

	std::pair<int, string> newMessageInfo = messages.front();
	messages.pop();

	finalLine += GetPortHandle(newMessageInfo.first, userHandles) + ':';

	if (newMessageInfo.second[0] == '1')
	{
		//  TODO - send plain message 
	}

	return finalLine;
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

	OutputWindow win;

	std::thread listeningThread;


	SocketAddressPtr listeningAddress;
	TCPSocketPtr listeningSocket;


	// the id and the relevant 
	std::map<int, string> userHandles;
	std::map<int, TCPSocketPtr> listeningConnectionSockets;
	std::map<int, TCPSocketPtr> sendingConnectionSockets;
	string userName;

	queue<std::pair<int, string>> unprocessedMessages;


	//conn->Receive();

	int answer = -1;
	
	int listeningPort;

	std::cout << "Which port would you like to use?\n";

	std::cin >> listeningPort;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	SetUpInitialListening(listeningPort, listeningSocket, listeningAddress);

	do 	// the main loop
	{
		bool shouldExitPreChatMenu;
		do  // pre-chat menu, for connecting/creating rooms
		{
			shouldExitPreChatMenu = true;


			do  // checking for valid menu choice
			{
				std::cout << "Would you like to:\n"
					<< " 1 - Make a session,\n"
					<< " 2 - Join a session, or\n"
					<< " 3 - Exit the program?\n";

				std::cin >> answer;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			} while (!(answer > 0 && answer < 4));

			switch (answer)
			{
			case 1:   // make a session
			{
				std::cout << "\n\n What username will you use?\n";
				std::cin >> userName;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				listeningThread = std::thread(HandleListening, listeningSocket, listeningConnectionSockets, unprocessedMessages);
				break;
			}

			case 2:   // join a session
			{
				int portToConnectTo;
				std::cout << "\n\n What is the port of the person you would like to connect to?\n";
				std::cin >> portToConnectTo;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				listeningThread = std::thread(HandleListening, listeningSocket, listeningConnectionSockets, unprocessedMessages);
				SetUpSending(portToConnectTo, listeningPort, sendingConnectionSockets);

				break;
			}


			case 3:  // quit
			{
				// TODO clean the connections up
				std::cout << "\n\nHave a nice day! (Press enter to exit).\n\n";
				string temp;
				std::cin >> temp;
				std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				exit(0);
				break;
			}

			default:
				std::cout << "\n\nERROR: INVALID MENU CHOICE GOT THROUGH.\n\n";
				exit(1);
				break;
			}

		} while (!shouldExitPreChatMenu);
		
		
		// ~~~~~~~~~~~~~~~~~~~  main menu now ~~~~~~~~~~~~~~~~~~~~~~~
		

		std::thread t([&win]()
			{
				int msgNo = 1;
				while (true)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(250));
					std::string msgIn("");
					std::stringstream ss(msgIn);
					//ss << msgNo;
					//ss << TODO - add the function here
					if(ss.str() != "")
					{
						win.Write(ss.str());
					}
					//msgNo++;
				}
			});

		while (true)
		{



			std::string input;
			getline(std::cin, input);
			win.WriteFromStdin(input);
		}

	} while (answer != 3);


	
	//t_Sending.join();
	//t_Listening.join();

	SocketUtil::CleanUp();

	return 0;
}
