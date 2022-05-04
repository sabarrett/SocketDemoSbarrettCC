
#include "RoboCatPCH.h"
#include <thread>
#include "RoboCat/Game.h"
#include"./RoboCat/EventSystem.h"
#include "./common/DeanLib/include/MemoryTracker.h"
#include "./RoboCat/NetworkManager.h"
#include "./RoboCat/InputSystem.h"
#include "./RoboCat/InputTranslator.h"

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

	//Assignment 2 code here
	NetworkManager* mpNetManager = new NetworkManager();
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
	/*
	if (serverInited || clientInited)
	{
		while (serverInited || clientInited)
		{
			if (mpInput->KEY_S)
			{
				mpNetManager->sendData(PacketTypes::CREATE_OBJECT, networkID);
			}
			else if (mpInput->KEY_D)
			{
				mpNetManager->sendData(PacketTypes::DESTROY_OBJECT, networkID);
			}

			mpNetManager->sendData(PacketTypes::UPDATE_OBJECT, networkID);
			mpNetManager->renderObject();
			mpNetManager->receiveData();
		}
	}
	*/

	system("pause");

	return 0;
}
