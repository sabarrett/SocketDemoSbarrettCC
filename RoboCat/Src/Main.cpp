
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>
#include "ChatUser.h"

#if _WIN32

ChatUser* chatter = new ChatUser();
const std::string SEND_PORT = "7000", RECV_PORT = "8080";
bool run = true;


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

	std::cout << "User 1 or User 2? Enter number: ";
	std::string userNumber = "1";
	std::getline(std::cin, userNumber);
	while (userNumber != "1" && userNumber != "2")
	{
		std::cout << "Invalid entry. Please enter 1 or 2: ";
		std::getline(std::cin, userNumber);
	}
	OutputWindow win1, win2;
	if (userNumber == "1")
	{
		chatter->username = "Chat User 1";
		std::thread c1([&win1]()
		{
			chatter->initTcpClient(SEND_PORT, RECV_PORT);
		});
	}
	else
	{
		chatter->username = "Chat User 2";
		std::thread c2([&win2]()
		{
			chatter->initTcpServer(RECV_PORT);
		});
	}



	while (run)
	{
		std::string input;
		std::getline(std::cin, input);
		if (input != "!EXIT")
		{
			chatter->win.WriteFromStdin("[" + chatter->username + "]: " + input + "\n");
			chatter->send(input);
		}
		else
		{
			run = false;
		}
	}

	SocketUtil::CleanUp();
	delete chatter;

	return 0;
}
