
#include "RoboCatPCH.h"

#include <thread>
#include <map>
#include "GraphicsLibrary.h"
#include "InputSystem.h"
#include "GameObjects.h"
#include "GameController.h"
#include "NetworkManager.h"
#include "PlayerController.h"
#include <time.h>

GraphicsLibrary* pGraphicsLib;
float screenSizeX = 1000.0;
float screenSizeY = 700.0;

InputSystem* pInput;
GameController* pGameController;

const std::string BACKGROUND_PATH = "Graphics\\field-bg.jpg";
const std::string BOULDER_PATH = "Graphics\\boulder-img.png";
const std::string BUBBLE_PATH = "Graphics\\bubble-img.png";
const std::string BEE_PATH = "Graphics\\bee-img.png";

const std::string BACKGROUND_IMG_IDENTIFIER = "background_img";
const std::string BOULDER_IMG_IDENTIFIER = "boulder_img";
const std::string BUBBLE_IMG_IDENTIFIER = "bubble_img";
const std::string BEE_IMG_IDENTIFIER = "bee_img";

PlayerController* playerServer;
PlayerController* playerClient;
ALLEGRO_EVENT_QUEUE* pEventQueue = nullptr;
ALLEGRO_TIMER* timer = nullptr;
NetworkManager* NetworkManager::mpNetworkInstance = 0;
NetworkManager* pNetworkManager;

bool isRunning = true;
float FPS = 60.0;
Colour blue(0, 0, 255); //P1
Colour red(255, 0, 0); //P2
int networkID = 0;
int dropOdds = 20;

int tempPlayerID;


bool init()
{
	bool initted = false;
	srand(time(0));

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
	if (tempPlayerID == 1)
	{
		tempPlayerID = 1;
		pNetworkManager->setServerRole(false);
		//playerServer = pNetworkManager->recieveInitFromServer(pGraphicsLib);
		//playerClient = pNetworkManager->recieveInitFromServer(pGraphicsLib);

		playerServer = new PlayerController(0, pGraphicsLib);
		playerServer->setPlayerID(0);
		pNetworkManager->spawnObj(playerServer, 0);
		//networkID++;

		playerClient = new PlayerController(1, pGraphicsLib);
		playerClient->setPlayerID(1);
		pNetworkManager->spawnObj(playerClient, 1);
		//networkID = 2;
		pNetworkManager->setCurrentID(1);
		pNetworkManager->mIsConnected = true;
	}
	else
	{
		tempPlayerID = 0;
		pNetworkManager->setServerRole(true);

		playerServer = new PlayerController(0, pGraphicsLib);
		playerServer->setPlayerID(0);
		pNetworkManager->spawnObj(playerServer, 0);
		//networkID++;

		//pNetworkManager->send(playerServer->getNetworkID(), TypePacket::PACKET_INIT);
		//pNetworkManager->recieve();


		playerClient = new PlayerController(1, pGraphicsLib);
		playerClient->setPlayerID(1);
		pNetworkManager->spawnObj(playerClient, 1);
		//pNetworkManager->recieve();
		//networkID++;
		//networkID = 2;
		pNetworkManager->setCurrentID(1);
		//pNetworkManager->send(playerClient->getNetworkID(), TypePacket::PACKET_INIT);
		pNetworkManager->mIsConnected = true;
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

	string imgID;

	switch (keyCode)
	{
		case KeyCode::B:
			imgID = BUBBLE_IMG_IDENTIFIER;
			break;

		case KeyCode::LEFT:
			imgID = BEE_IMG_IDENTIFIER;
			break;

		case KeyCode::RIGHT:
			imgID = "";
			break;

		case KeyCode::SPACE:
			imgID = BOULDER_IMG_IDENTIFIER;
			break;

		case KeyCode::ESC:
			isRunning = false;
			break;
	}

	if(pNetworkManager->getServerRole())
		pNetworkManager->processPacket(keyCode, imgID, pNetworkManager->getServerRole());
	else
		pNetworkManager->requestPacket(keyCode);

	pNetworkManager->updateObj();

	Timing::sInstance.Update();
	pNetworkManager->update(Timing::sInstance.GetDeltaTime(), Timing::sInstance.GetTime());

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
		pNetworkManager->init(pGraphicsLib, blue, red, dropOdds);

		std::string role;
		std::cout << "Are you the host? 'y' or 'n'\n";
		std::cin >> role;

		if (role == "y")
		{
			std::string port;
			port = "8080";

			successConnect = pNetworkManager->initServer(port);
			if (successConnect)
				std::cout << "connect successful.\n";

			tempPlayerID = 0;
			//networkID = 0;
		}

		else if (role == "n")
		{
			std::string connectIP;
			connectIP = "127.0.0.1";

			std::string port;
			port = "8080";


			successConnect = pNetworkManager->connect(connectIP, port);
			if (successConnect)
				std::cout << "Client Connected\n";

			tempPlayerID = 1;
			//networkID = 1;
		}

		if (successConnect)
		{
			start();
			//networkID = 2; //don't overwrite players

			while (isRunning)
			{
				ALLEGRO_EVENT alEvent;
				al_get_next_event(pEventQueue, &alEvent);

				if (alEvent.type == ALLEGRO_EVENT_TIMER)
				{
					update();
					pNetworkManager->send(playerServer->getNetworkID(), TypePacket::PACKET_UPDATE);

					pNetworkManager->recieve();
					draw();
				}
			}
			cleanup();
		}
	}

	system("pause");

	return 0;
}

