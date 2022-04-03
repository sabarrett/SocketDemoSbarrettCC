
#include "RoboCatPCH.h"

#include <thread>
#include <map>
#include "GraphicsLibrary.h"
#include "InputSystem.h"
#include "GameObjects.h"
#include "GameController.h"
#include "NetworkManager.h"
#include "PlayerController.h"

GraphicsLibrary* pGraphicsLib;
float screenSizeX = 1000.0;
float screenSizeY = 700.0;

InputSystem* pInput;
GameController* pGameController;

const std::string BACKGROUND_PATH = "Graphics\\field-bg.jpg";
const std::string BOULDER_PATH = "Graphics\\boulder-ing.png";
const std::string BUBBLE_PATH = "Graphics\\bubble-ing.png";
const std::string BEE_PATH = "Graphics\\bee-ing.png";

const std::string BACKGROUND_IMG_IDENTIFIER = "background_img";
const std::string BOULDER_IMG_IDENTIFIER = "boulder_img";
const std::string BUBBLE_IMG_IDENTIFIER = "bubble_img";
const std::string BEE_IMG_IDENTIFIER = "bee_img";

Colour blue(0, 0, 255); //P1
Colour red(255, 0, 0); //P2

bool isRunning = true;
float FPS = 60.0;

ALLEGRO_EVENT_QUEUE* pEventQueue = nullptr;
ALLEGRO_TIMER* timer = nullptr;

PlayerController* player;

NetworkManager* NetworkManager::mpNetworkInstance = 0;
NetworkManager* pNetworkManager;
int networkID = 0;

bool init()
{
	bool initted = false;

	pGraphicsLib = new GraphicsLibrary(screenSizeX, screenSizeY);
	initted = pGraphicsLib->init();

	pGraphicsLib->loadImage(BACKGROUND_PATH, BACKGROUND_IMG_IDENTIFIER);
	pGraphicsLib->loadImage(BOULDER_PATH, BOULDER_IMG_IDENTIFIER);
	pGraphicsLib->loadImage(BEE_PATH, BEE_IMG_IDENTIFIER);
	pGraphicsLib->loadImage(BUBBLE_PATH, BUBBLE_IMG_IDENTIFIER);

	pInput = new InputSystem();
	if (initted)
		initted = pInput->init(pGraphicsLib);

	timer = al_create_timer(1.0 / FPS);
	pEventQueue = al_create_event_queue();
	al_register_event_source(pEventQueue, al_get_timer_event_source(timer));

	return initted;
}

void start()
{
	//server
	if (networkID == 0)
	{
		player = new PlayerController(networkID, pGraphicsLib);
		pNetworkManager->spawnObj(player, networkID);
		networkID++;

		pNetworkManager->send(player->getNetworkID(), Packets::PACKET_CREATE);
		pNetworkManager->recieve();
	}
	//client
	else if (networkID == 1)
	{
		pNetworkManager->recieve();
		player = new PlayerController(networkID, pGraphicsLib);
		pNetworkManager->spawnObj(player, networkID);
		networkID++;

		pNetworkManager->send(player->getNetworkID(), Packets::PACKET_CREATE);
	}

	al_start_timer(timer);
}

void draw()
{
	pGraphicsLib->drawImage(BACKGROUND_IMG_IDENTIFIER, 0, 0);

	pNetworkManager->renderObj();

	pGraphicsLib->render();
}

void update()
{
	KeyCode keyCode = pInput->getKeyboardInput(InputMode::KeyPressed);
	switch (keyCode)
	{
	case KeyCode::ESC:
	{
		isRunning = false;
		break;
	}
	case KeyCode::B:
	{
		pGameController->makeBubble(networkID);
		pNetworkManager->send(networkID, Packets::PACKET_CREATE);
		break;
	}
	}

	pNetworkManager->updateObj();
}

void cleanup()
{

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
	bool runGame = true;
	if (init())
	{
		bool successConnect = false;
		pNetworkManager = pNetworkManager->GetInstance();
		pNetworkManager->init(pGraphicsLib, blue, red);

		std::string role;
		std::cout << "Are you the host? 'y' or 'n'\n";
		std::cin >> role;

		if (role == "y")
		{
			std::string port;
			port = "8080";
			/*std::cout << "Port: ";
			std::cin >> port;*/

			successConnect = pNetworkManager->initServer(port);
			if (successConnect)
				std::cout << "connect successful.\n";

			networkID = 0;
		}

		else if (role == "n")
		{
			std::string connectIP;
			connectIP = "127.0.0.1";
			/*std::cout << "Server IP: ";
			std::cin >> connectIP;*/

			std::string port;
			port = "8080";
			/*std::cout << "Port: ";
			std::cin >> port;*/

			successConnect = pNetworkManager->connect(connectIP, port);
			if (successConnect)
				std::cout << "Client Connected\n";

			networkID = 1;
		}

		if (successConnect)
		{
			start();
			networkID = 2;

			while (isRunning)
			{
				ALLEGRO_EVENT alEvent;
				al_get_next_event(pEventQueue, &alEvent);

				if (alEvent.type == ALLEGRO_EVENT_TIMER)
				{
					update();
					pNetworkManager->send(player->getNetworkID(), Packets::PACKET_UPDATE);

					pNetworkManager->recieve();
					draw();
				}
			}
			cleanup();
		}
	}





	//SocketUtil::StaticInit();

	//UDPSocketPtr cliSock = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	//UDPSocketPtr srvSock = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);

	//SocketAddressPtr srvAddr = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:9001");

	////clientAddr not needed for long, don't keep it around
	//{
	//	SocketAddressPtr cliAddr = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:9000");
	//	if (cliAddr == nullptr)
	//	{
	//		SocketUtil::ReportError("create client socket");
	//		ExitProcess(1);
	//	}
	//	cliSock->Bind(*cliAddr);
	//}
	//srvSock->Bind(*srvAddr);

	//std::string msg("Hello Server");
	//int nBytesSent = cliSock->SendTo(msg.c_str(), msg.length(), *srvAddr);
	//if (nBytesSent <= 0)
	//{
	//	SocketUtil::ReportError("Client SendTo");
	//}
	//std::cout << "Sent " << nBytesSent << " bytes\n";
	//

	//std::thread srvThread([&srvSock]()
	//{
	//	char buffer[4096];
	//	SocketAddress fromAddr;
	//	int nBytesRecieved = srvSock->ReceiveFrom(buffer, 4096, fromAddr);
	//	if (nBytesRecieved <= 0)
	//	{
	//		SocketUtil::ReportError("Server RecieveFrom");
	//		return;
	//	}
	//	std::string msg(buffer, nBytesRecieved);
	//	std::cout << "Recieved message from: " << fromAddr.ToString() << ": " << msg << std::endl;
	//});


	//srvThread.join();

	//SocketUtil::CleanUp();

	system("pause");

	return 0;
}

