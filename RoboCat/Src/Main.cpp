#pragma once
#pragma warning (disable : 4996)

#include "Client/Game.h"
#include "Server/Server.h"

#include <thread>






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
		std::string launchCommand;
		std::getline(std::cin, launchCommand);
		char* inputC = new char[launchCommand.length() + 1];
		strcpy(inputC, launchCommand.c_str());
		char delim[] = " ";
		char* ptr = strtok(inputC, delim);
		while (ptr != NULL)
		{
			if (strcmp(ptr, "/connect") == 0)
			{
				ptr = strtok(NULL, delim);
				std::string newAddress(ptr);
				ptr = strtok(NULL, delim);
				std::string newPort(ptr);

				game->ConnectToServer(newAddress, newPort);

				ptr = NULL;
				enteredValidCommand = true;
			}
			else if (ptr != NULL && strcmp(ptr, "/host") == 0)
			{
				server = new Server;
				int port = server->Init();

				game->ConnectToServer("127.0.0.1", StringUtils::Sprintf("%d", port));

				ptr = NULL;
				enteredValidCommand = true;
			}
			else
			{
				ptr = NULL;
			}
		}
	}

	while (game->Running())
	{
		frameStart = SDL_GetTicks();

		if (server)
		{
			server->Update();
		}
		game->Update();
		game->Draw();
		game->DoNetworking();

		frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime)
		{
			SDL_Delay(frameDelay - frameTime);
		}
		frame++;
	}
	if (server)
	{
		server->CleanUp();
	}
	game->CleanUp();

	

	SocketUtil::CleanUp();

	return 0;
}


//UDPSocketPtr clientSocket = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
//UDPSocketPtr serverSocket = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
//
//SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:65000");
//SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:65001");
//
//clientSocket->Bind(*clientAddress);
//serverSocket->Bind(*serverAddress);
//
//std::thread serverThread([&serverSocket]() {
//	char buffer[4096];
//	SocketAddress fromAddress;
//	int bytesReceived = serverSocket->ReceiveFrom(buffer, 4096, fromAddress);
//	if (bytesReceived <= 0)
//	{
//		SocketUtil::ReportError("ByesRecieved <= 0");
//		return;
//	}
//	std::string msg(buffer, bytesReceived);
//	std::cout << fromAddress.ToString() << ": " << msg.c_str() << "\n";
//
//	});
//
//
//std::string msg("Hello, sir.");
//int bytesSent = clientSocket->SendTo(msg.c_str(), msg.length(), *serverAddress);
//std::cout << bytesSent << "\n";
//if (bytesSent <= 0)
//{
//	SocketUtil::ReportError("None Sended");
//}
//
//serverThread.join();