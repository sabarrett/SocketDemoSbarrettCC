
#include "RoboCatPCH.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>
#include "ChatUser.h"

#if _WIN32

ChatUser* chatter = new ChatUser();
std::thread tr1, tr2;
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
	
	chatter->startTcpThread(userNumber == "1");	
	std::cout << "Enter \"!EXIT\" at any point to exit. Case sensitive.\n";

	while (run)
	{
		std::string input;
		std::getline(std::cin, input);
		
		chatter->win.WriteFromStdin("[" + chatter->username + "]: " + input + "\n");
		if (input == "!EXIT")
			run = false;
		else
			chatter->send(input);
	}

	delete chatter;

	return 0;
}
