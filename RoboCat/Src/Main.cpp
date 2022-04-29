
#include "RoboCatPCH.h"
#include <thread>
#include "RoboCat/Game.h"
#include"./RoboCat/EventSystem.h"
#include "./common/DeanLib/include/MemoryTracker.h"
#include "./RoboCat/NetworkManager.h"
#include "./RoboCat/InputSystem.h"

#if _WIN32

void UDPServer(UDPSocketPtr server)
{
	std::thread sendInput([&] {

	});

	std::thread update([&] {

	});

	sendInput.join();
	update.join();
}

void UDPClient(UDPSocketPtr client)
{
	std::thread sendInput([&] {

	});

	std::thread update([&] {

	});

	sendInput.join();
	update.join();
}

const int DISPLAY_WIDTH = 800;
const int DISPLAY_HEIGHT = 600;

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

	std::string serverPort = "8080";
	std::string clientIP = "127.0.0.1";
	std::string clientPort = "8081";

	bool serverInited;
	bool clientInited;

	int networkID = 0;
	bool portnumber = StringUtils::GetCommandLineArg(1) == "8080";

	if (portnumber) {

		serverInited = mpNetManager->initServer(serverPort);

	}
	else
	{
		
		clientInited = mpNetManager->connect(clientIP, serverPort);

	}
	
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

			else
			{
				mpNetManager->sendData(PacketTypes::UPDATE_OBJECT, networkID);
			}

			mpNetManager->receiveData();
		}
	}

	system("pause");

	return 0;
}
