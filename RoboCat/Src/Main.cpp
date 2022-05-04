
#include "RoboCatPCH.h"
#include <thread>
#include "RoboCat/Game.h"
#include"./RoboCat/EventSystem.h"
#include "./common/DeanLib/include/MemoryTracker.h"
#include "./RoboCat/NetworkManager.h"
#include "./RoboCat/InputSystem.h"
#include "./RoboCat/InputTranslator.h"
#include <time.h>
#include <stdlib.h>

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
	srand(time(0));
	int dropChance = rand() % 101;

	//Assignment 2 code here
	NetworkManager* mpNetManager = new NetworkManager(dropChance);
	InputSystem* mpInput = new InputSystem();
	InputTranslator* mpTranslator = new InputTranslator(mpInput);

	std::string serverPort = "8080";
	std::string clientIP = "127.0.0.1";
	std::string clientPort = "8081";

	bool serverInited = false;
	bool clientInited = false;

	int networkID;
	bool portnumber = StringUtils::GetCommandLineArg(1) == "8080";

	if (portnumber) 
	{
		serverInited = mpNetManager->initServer(serverPort);
		networkID = 0;
	}
	else
	{
		clientInited = mpNetManager->connect(clientIP, serverPort);
		networkID = 1;
	}

	system("pause");

	return 0;
}
