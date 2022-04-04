
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
const std::string BOULDER_PATH = "Graphics\\boulder-img.png";
const std::string BUBBLE_PATH = "Graphics\\bubble-ing.png";
const std::string BEE_PATH = "Graphics\\bee-ing.png";

const std::string BACKGROUND_IMG_IDENTIFIER = "background_img";
const std::string BOULDER_IMG_IDENTIFIER = "boulder_img";
const std::string BUBBLE_IMG_IDENTIFIER = "bubble_img";
const std::string BEE_IMG_IDENTIFIER = "bee_img";

PlayerController* player;
ALLEGRO_EVENT_QUEUE* pEventQueue = nullptr;
ALLEGRO_TIMER* timer = nullptr;
NetworkManager* NetworkManager::mpNetworkInstance = 0;
NetworkManager* pNetworkManager;

bool isRunning = true;
float FPS = 60.0;
Colour blue(0, 0, 255); //P1
Colour red(255, 0, 0); //P2
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

		pNetworkManager->send(player->getNetworkID(), TypePacket::PACKET_CREATE);
		pNetworkManager->recieve();
	}

	//client
	else if (networkID == 1)
	{
		player = new PlayerController(networkID, pGraphicsLib);
		pNetworkManager->recieve();
		pNetworkManager->spawnObj(player, networkID);
		networkID++;

		pNetworkManager->send(player->getNetworkID(), TypePacket::PACKET_CREATE);
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
			//make bubble
			pNetworkManager->send(networkID, TypePacket::PACKET_CREATE);
			break;
		}

		case KeyCode::LEFT:
		{
			//make left bees
			pNetworkManager->send(networkID, TypePacket::PACKET_CREATE);
			break;
		}
	
		case KeyCode::RIGHT:
		{	
			//make right bees
			pNetworkManager->send(networkID, TypePacket::PACKET_CREATE);
			break;
		}

		case KeyCode::SPACE:
		{	
			float randPosX = 0.0;
			float randPosY =0.0;

			GameObjects* newBoulder;
			newBoulder = new Boulder(pGraphicsLib, networkID, BOULDER_IMG_IDENTIFIER, randPosX, randPosY);

			pNetworkManager->spawnObj(newBoulder, networkID);
			pNetworkManager->send(networkID, TypePacket::PACKET_CREATE);
			networkID++;
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
	bool successConnect = false;
	if (init())
	{
		pNetworkManager = pNetworkManager->GetInstance();
		pNetworkManager->init(pGraphicsLib, blue, red);

		std::string role;
		std::cout << "Are you the host? 'y' or 'n'\n";
		std::cin >> role;

		if (role == "y")
		{
			std::string port;
			port = "8080"; //can change later

			successConnect = pNetworkManager->initServer(port);
			if (successConnect)
				std::cout << "connect successful.\n";

			networkID = 0;
		}

		else if (role == "n")
		{
			std::string connectIP;
			connectIP = "127.0.0.1"; //can change later

			std::string port;
			port = "8080"; //can change later


			successConnect = pNetworkManager->connect(connectIP, port);
			if (successConnect)
				std::cout << "Client Connected\n";

			networkID = 1;
		}

		if (successConnect)
		{
			start();
			networkID = 2; //don't overwrite players

			while (isRunning)
			{
				ALLEGRO_EVENT alEvent;
				al_get_next_event(pEventQueue, &alEvent);

				if (alEvent.type == ALLEGRO_EVENT_TIMER)
				{
					update();
					pNetworkManager->send(player->getNetworkID(), TypePacket::PACKET_UPDATE);

					pNetworkManager->recieve();
					draw();
				}
			}
			cleanup();
		}
	}

	//SocketUtil::CleanUp();

	system("pause");

	return 0;
}

