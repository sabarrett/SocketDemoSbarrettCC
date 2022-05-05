#include "RoboCatPCH.h"
#include <thread>
#include <iostream>
#include "PlayerUser.h"


PlayerUser* user;
bool isRunning = true;
//std::thread t;


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

	std::string username = "charles entertainment cheese";

	// Server Code --------------
	bool isServer = StringUtils::GetCommandLineArg(1) == "server";
	if (isServer)
	{
		username = "server";
		user = new PlayerUser(2, username);
		user->startTcpThread(isServer);
	}

	// Client Code --------------
	if (!isServer)
	{
		std::cout << "Enter username please:\n";
		std::getline(std::cin, username);
		user = new PlayerUser(1, username);
		user->startTcpThread(isServer);
	}

	std::cout << "Welcome, " + username + "!\nEnter !quit to exit.\n";
	std::cout << "< " + user->getUsername() + " >: ";
	while (isRunning)
	{
		std::string input;
		
		std::getline(std::cin, input);

		std::cout << "< " + user->getUsername() + " >: ";
		std::cout << input << "\n";
		if (input == "!quit" || input == "!Quit" || input == "!QUIT")
			isRunning = false;
		else
			user->sendMessage(input);
	}

	delete user;

	SocketUtil::CleanUp();

	return 0;
}

