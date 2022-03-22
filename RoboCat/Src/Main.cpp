#include "RoboCatPCH.h"
#include "Game.h"

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

	const int FPS = 60;
	const int frameDelay = 1000 / FPS;

	Uint32 frameStart;
	int frameTime;

	Game* game = new Game();

	game->Init("Heya", 1600, 900);

	while (game->Running())
	{
		frameStart = SDL_GetTicks();

		game->Update();
		game->Draw();

		frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime)
		{
			SDL_Delay(frameDelay - frameTime);
		}

	}
	
	game->CleanUp();

	SocketUtil::StaticInit();

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