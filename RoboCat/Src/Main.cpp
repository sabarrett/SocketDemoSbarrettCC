#include <map>
#include <iostream>

#include "Networker.h"

#include "InputSystem.h"
#include "GraphicsLibrary.h"
#include "GameObject.h"
#include "Rock.h"
#include "Wall.h"
#include "PlayerController.h"

//-------------------------Graphics Data-------------------------
GraphicsLibrary* pGraphics;
float screenSizeX = 1600.0;
float screenSizeY = 900.0;

//-------------------------Input Data-------------------------
InputSystem* pInput;

//-------------------------Assets-------------------------
const std::string ASSET_PATH = "Assets\\";
const std::string BACKGROUND_IMAGE_FILE = "Background_Image.jpg";
const std::string ROCK_IMAGE_FILE = "Rock_Image.png";
const std::string WALL_IMAGE_FILE = "Square_Image.png";
const std::string PLAYER_IMAGE_FILE = "Player_Image.png";
const std::string ARIAL_FONT_FILE = "ARIBL0.ttf";
const int FONT_SIZE = 32;

//-------------------------Colours-------------------------
Colour white(255, 255, 255, 255);

//-------------------------Asset Identifiers-------------------------
const std::string BACKGROUND_IMAGE_SPRITE_IDENTIFIER = "background_image";
const std::string ROCK_SPRITE_IDENTIFIER = "rock_image";
const std::string WALL_SPRITE_IDENTIFIER = "wall_image";
const std::string PLAYER_SPRITE_IDENTIFIER = "player_image";

//-------------------------Game Data-------------------------
bool bGameIsRunning = true;
std::vector<GameObject*> gameObjectsVec;
std::map<int, GameObject*> gameObjectMap;

float FPS = 60;
ALLEGRO_TIMER* timer = nullptr;
ALLEGRO_EVENT_QUEUE* eventQueue = nullptr;

float wallSizeX = 150;
float wallSizeY = 15;
float wallBorderThickness = 5.0;

//-------------------------GameObject Data-------------------------
GameObjectType currentGameObjectType;
std::string currentGameObjectTypeString;
int gameObjectID = 0;

//-------------------------Player Data-------------------------
PlayerController* pPlayerController;
const std::pair<float, float> STARTING_PLAYER_POSITION = std::make_pair<float, float>(300.0, 300.0);
float playerMoveSpeed = 0.5;

//-------------------------Network Data-------------------------
Networker* Networker::mInstance = 0;
Networker* pNetworkManager;
int networkID = 0;

bool init()
{
	bool bSuccessfulInit = false;

	//Setup the graphical window
	pGraphics = new GraphicsLibrary(screenSizeX, screenSizeY);
	bSuccessfulInit = pGraphics->init();

	//Setup text
	if (bSuccessfulInit)
		bSuccessfulInit = pGraphics->initText(ASSET_PATH + ARIAL_FONT_FILE, FONT_SIZE, white);

	//Add images to the graphcis library
	pGraphics->loadImage(ASSET_PATH + BACKGROUND_IMAGE_FILE, BACKGROUND_IMAGE_SPRITE_IDENTIFIER);
	pGraphics->loadImage(ASSET_PATH + ROCK_IMAGE_FILE, ROCK_SPRITE_IDENTIFIER);
	pGraphics->loadImage(ASSET_PATH + WALL_IMAGE_FILE, WALL_SPRITE_IDENTIFIER);
	pGraphics->loadImage(ASSET_PATH + PLAYER_IMAGE_FILE, PLAYER_SPRITE_IDENTIFIER);

	//Setup the input system
	pInput = new InputSystem();
	if (bSuccessfulInit)
		bSuccessfulInit = pInput->init(pGraphics);

	//Setup timer
	timer = al_create_timer(1.0 / FPS);
	eventQueue = al_create_event_queue();
	al_register_event_source(eventQueue, al_get_timer_event_source(timer));

	//Init and return if it succeeded or not
	return bSuccessfulInit;
}

void start()
{
	std::cout << "Hitting start()\n";
	//Default GameObject to spawn
	currentGameObjectType = GameObjectType::ROCK;
	currentGameObjectTypeString = "Rock";

	//Spawn player
	pPlayerController = new PlayerController(gameObjectID, networkID, pGraphics, STARTING_PLAYER_POSITION, playerMoveSpeed, PLAYER_SPRITE_IDENTIFIER);
	gameObjectID++;
	networkID++;

	al_start_timer(timer);
}

void update()
{
	std::cout << "Hitting update()\n";

	//Get mouse input
	{
		MouseButton mouseInput = pInput->getMouseInput();

		//Handle input
		switch (mouseInput)
		{
		case MouseButton::LeftMouse:
		{
			//Spawn current GameObject type
			GameObject* gameObjectToSpawn;

			switch (currentGameObjectType)
			{
			case GameObjectType::ROCK:
			{
				pair<float, float> mousePos = std::make_pair(pInput->getMouseX(), pInput->getMouseY());
				gameObjectToSpawn = dynamic_cast<GameObject*>(new Rock(gameObjectID, networkID, pGraphics, mousePos, ROCK_SPRITE_IDENTIFIER));

				break;
			}
			case GameObjectType::WALL:
			{
				pair<float, float> mousePos = std::make_pair(pInput->getMouseX(), pInput->getMouseY());
				gameObjectToSpawn = dynamic_cast<GameObject*>(new Wall(gameObjectID, networkID, pGraphics, mousePos, wallSizeX, wallSizeY, white, wallBorderThickness));

				break;
			}

			default:
			{
				std::cout << "INVALID GAMEOBJECT TYPE! CANNOT CREATE!\n";

				break;
			}
			}

			gameObjectMap[gameObjectID] = gameObjectToSpawn;
			gameObjectToSpawn = nullptr;

			//Increment identifiers
			gameObjectID++;
			networkID++;
		}

		default:
			break;
		}
	}

	//Get keyboard input
	{
		KeyCode keyCode = pInput->getKeyboardInput(InputMode::KeyPressed);

		switch (keyCode)
		{
		case KeyCode::R:
		{


			break;
		}

		case KeyCode::Tab:
		{
			//Cycle throught GameObject types
			currentGameObjectType = static_cast<GameObjectType>((currentGameObjectType + 1) % GameObjectType::ENUM_SIZE);

			//DO NOT SPAWN A PLAYER
			if (currentGameObjectType == GameObjectType::PLAYER)
				currentGameObjectType = static_cast<GameObjectType>((currentGameObjectType + 1) % GameObjectType::ENUM_SIZE);

			switch (currentGameObjectType)
			{
			case GameObjectType::INVALID:
				currentGameObjectTypeString = "INVALID";
				break;

			case GameObjectType::ROCK:
				currentGameObjectTypeString = "Rock";
				break;

			case GameObjectType::WALL:
				currentGameObjectTypeString = "Wall";
				break;

			default:
				currentGameObjectTypeString = "INVALID";
				break;
			}

			break;
		}

		case KeyCode::Esc:
		{
			//Quit game
			bGameIsRunning = false;
			return;
			break;
		}

		default:
			break;
		}
	}

	//Update GameObjects
	for (const auto& x : gameObjectMap)
	{
		x.second->update();
	}

	//Update player controller
	pPlayerController->update();
}

void draw()
{
	std::cout << "Hitting draw()\n";

	//Background image
	pGraphics->drawImage(BACKGROUND_IMAGE_SPRITE_IDENTIFIER, 0, 0);

	//Draw GameObjects
	for (const auto& x : gameObjectMap)
	{
		x.second->draw();
	}

	//Draw player controller
	pPlayerController->draw();

	//Text indicator of current GameObject Type
	pGraphics->drawText(100, 50, "Current Object to Spawn: " + currentGameObjectTypeString, TextAlignment::ALIGN_LEFT);

	//Text indicators for instructions
	{
		pGraphics->drawText(pGraphics->getScreenSizeX() - 700, 50, "WASD - Move Player.", TextAlignment::ALIGN_LEFT);
		pGraphics->drawText(pGraphics->getScreenSizeX() - 700, 100, "TAB - Change GameObject to Spawn.", TextAlignment::ALIGN_LEFT);
		pGraphics->drawText(pGraphics->getScreenSizeX() - 700, 150, "Left Mouse - Spawn GameObject.", TextAlignment::ALIGN_LEFT);
		pGraphics->drawText(pGraphics->getScreenSizeX() - 700, 200, "ESC - Quit.", TextAlignment::ALIGN_LEFT);
	}

	//Render it all
	pGraphics->render();
}

void cleanup()
{
	std::cout << "Hitting cleanup()\n";

	//cleanup timer
	al_destroy_timer(timer);
	al_destroy_event_queue(eventQueue);

	//Cleanup GameObjects
	for (const auto& x : gameObjectMap)
	{
		delete x.second;
		gameObjectMap.erase(x.first);
	}
	gameObjectMap.clear();

	//Cleanup player
	delete pPlayerController;
	pPlayerController = nullptr;

	//Cleanup network manager
	pNetworkManager->cleanup();
	pNetworkManager = nullptr;

	//Cleanup input system
	delete pInput;
	pInput = nullptr;

	//Cleanup the graphics system
	delete pGraphics;
	pGraphics = nullptr;
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

	if (init())
	{
		//Setup network manager
		pNetworkManager = pNetworkManager->GetInstance();
		pNetworkManager->init();

		//Prompt for isServer or not
		std::string input;
		std::cout << "Are you the server? Type 'y' for yes, anything else for no.\n";
		std::cin >> input;
		bool bIsServer = false;
		bool bHasConnectd = false;

		if (input == "y")
		{
			bIsServer = true;
		}

		//Setup server and client
		if (bIsServer)
		{
			//-------------------------Server code-------------------------

			//Prompt for port number
			std::string portNumber;
			std::cout << "Enter port number: \n";
			std::cin >> portNumber;

			bHasConnectd = pNetworkManager->initServer(portNumber);
			if (bHasConnectd)
				std::cout << "main.cpp --> server initted.\n";
		}
		else
		{
			//-------------------------Client code-------------------------

			//Prompt for client IP address
			std::string clientIP;
			std::cout << "Enter your IP address: \n";
			std::cin >> clientIP;

			//Prompt for server IP address
			std::string serverIP;
			std::cout << "Enter server IP address: \n";
			std::cin >> serverIP;

			//Prompt for port number
			std::string portNumber;
			std::cout << "Enter port number: \n";
			std::cin >> portNumber;

			bHasConnectd = pNetworkManager->connect(clientIP, serverIP, portNumber);
			if (bHasConnectd)
				std::cout << "main.cpp --> client connected.\n";
		}

		//If the peer has connected
		if (bHasConnectd)
		{
			//Setup
			start();

			//Loop the game
			while (bGameIsRunning)
			{
				ALLEGRO_EVENT ev;
				al_get_next_event(eventQueue, &ev);

				if (ev.type == ALLEGRO_EVENT_TIMER)
				{
					//Update call
					update();

					//Draw call
					draw();
				}

				std::cout << "bGameIsRunning loop\n";
			}

			//Cleanup when done
			cleanup();
		}
	}

	std::cout << "End of program.\n";
	std::cin.get();

	return 0;
}
