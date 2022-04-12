#pragma once
#pragma warning (disable : 4996)

#include "Client/Game.h"
#include "Server/Server.h"

#include <thread>

bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
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

	const int FPS = 144;
	const int frameDelay = 1000 / FPS;

	Uint32 frameStart;
	int frameTime;
	int frame = 0;

	Game* game = new Game();
	Server* server = nullptr;

	SocketUtil::StaticInit();

	game->Init("Network Game", 1600, 900);

	bool enteredValidCommand = false;
	while (!enteredValidCommand)
	{
		std::cout
			<< "TYPE \"/connect x.x.x.x portNumber\" to connect to server\n"
			<< "TYPE \"/host to host server\n";
		string space_delimiter = " ";
		std::string command;
		std::getline(std::cin, command);
		vector<string> cinInput{};

		size_t pos = 0;
		while ((pos = command.find(space_delimiter)) != string::npos) 
		{
			cinInput.push_back(command.substr(0, pos));
			command.erase(0, pos + space_delimiter.length());
		}
		cinInput.push_back(command.substr(0, pos));
		if (cinInput[0] == "/connect" && cinInput.size() >= 3)
		{
			game->ConnectToServer(cinInput[1], cinInput[2]);
			enteredValidCommand = true;
		}
		if (cinInput[0] == "/host" && cinInput.size() >= 2)
		{
			server = new Server;

			int port = stoi(cinInput[1]);
			server->Init(port);
			game->ConnectToServer("127.0.0.1", StringUtils::Sprintf("%d", port));
			enteredValidCommand = true;
		}

	}

	std::thread cinThread([&game, server]() 
		{
		std::cout
			<< "TYPE \"/latency x\" to set latency in seconds\n"
			<< "TYPE \"/jitter x\" to set max jitter in seconds\n"
			<< "TYPE \"/drop x\" to set drop chance 0-1, with 1 being 100%\n";
		    
		if (server)
		{
			std::cout << "And change the server settings with:\n"
				<< "\"/slatency x\"\n"
				<< "\"/sjitter x\"\n"
				<< "\"/sdrop x\"\n";
		}
		while (game->Running())
		{
			string space_delimiter = " ";
			std::string command;
			std::getline(std::cin, command);
			vector<string> cinInput{};

			size_t pos = 0;
			while ((pos = command.find(space_delimiter)) != string::npos) {
				cinInput.push_back(command.substr(0, pos));
				command.erase(0, pos + space_delimiter.length());
			}
			cinInput.push_back(command.substr(0, pos));
			if (cinInput[0] == "/latency" && cinInput.size() >= 2)
			{
				if (is_number(cinInput[1]))
				{
					game->SetLatency(stof(cinInput[1]));
				}
			}
			if (cinInput[0] == "/jitter" && cinInput.size() >= 2)
			{
				if (is_number(cinInput[1]))
				{
					game->SetJitter(stof(cinInput[1]));
				}
			}
			if (cinInput[0] == "/drop" && cinInput.size() >= 2)
			{
				if (is_number(cinInput[1]))
				{
					game->SetDrop(stof(cinInput[1]));
				}
			}
			if (cinInput[0] == "/slatency" && cinInput.size() >= 2 && server)
			{
				if (is_number(cinInput[1]))
				{
					server->SetLatency(stof(cinInput[1]));
				}
			}
			if (cinInput[0] == "/sjitter" && cinInput.size() >= 2 && server)
			{
				if (is_number(cinInput[1]))
				{
					server->SetJitter(stof(cinInput[1]));
				}
			}
			if (cinInput[0] == "/sdrop" && cinInput.size() >= 2 && server)
			{
				if (is_number(cinInput[1]))
				{
					server->SetDrop(stof(cinInput[1]));
				}
			}
		}
		});
	



	while (game->Running())
	{
		frameStart = SDL_GetTicks();
		Timing::sInstance.Update();
		if (server)
		{
			server->Update();
		}
		game->Update();
		game->Draw();

		frameTime = SDL_GetTicks() - frameStart;
		while(frameDelay > frameTime)
		{
			frameTime = SDL_GetTicks() - frameStart;
		}
		frame++;
	}
	if (server)
	{
		server->CleanUp();
	}
	game->CleanUp();

	SocketUtil::CleanUp();


	std::cout << "\n\n\nGame Closed, enter anything to confirm.\n";
	cinThread.join();

	return 0;
}