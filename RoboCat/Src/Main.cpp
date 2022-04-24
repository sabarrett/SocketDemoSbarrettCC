#include <iostream>


#include "..\Game.h"



int main(int argc, const char** argv)
{

	bool isServer = false;
	string input;

	std::cout << "Would you like to host or join(Join/Host)? " << std::endl;
	std::cin >> input;
	
	while (input != "Join" && input != "Host")
	{
		std::cout << "Please enter a valid answer " << std::endl;
		std::cout << "Would you like to host or join(Join/Host)? " << std::endl;
		std::cin >> input;

	}

	Game* game;

	if (input == "Host")
	{
		isServer = true;
		game = new Game();
	}
	else
	{
		isServer = false;

		std::cout << "Enter an IP address to connect to? " << std::endl;
		std::cin >> input;
		game = new Game(input);
	}

	
	game->Update();

	return 0;
}

//SocketUtil::CleanUp();