//#include <map>
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
bool bCanSpawnGameObject = false;

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
Colour wallColour;

//-------------------------Asset Identifiers-------------------------
const std::string BACKGROUND_IMAGE_SPRITE_IDENTIFIER = "background_image";
const std::string ROCK_SPRITE_IDENTIFIER = "rock_image";
const std::string WALL_SPRITE_IDENTIFIER = "wall_image";
const std::string PLAYER_SPRITE_IDENTIFIER = "player_image";

//-------------------------Game Data-------------------------
bool bGameIsRunning = true;
std::map<int, GameObject*> gameObjectMap;

int FPS = 60;
ALLEGRO_TIMER* timer = nullptr;
ALLEGRO_EVENT_QUEUE* eventQueue = nullptr;

float wallSizeX = 150;
float wallSizeY = 15;
float wallBorderThickness = 5.0;

//-------------------------GameObject Data-------------------------
GameObjectType currentGameObjectType;
std::string currentGameObjectTypeString;

//-------------------------Player Data-------------------------
PlayerController* pPlayerController;

std::pair<float, float> startingPlayerPos;
const std::pair<float, float> STARTING_PLAYER_POSITION_SERVER = std::make_pair<float, float>(300.0, 300.0);
const std::pair<float, float> STARTING_PLAYER_POSITION_CLIENT = std::make_pair<float, float>(900.0, 300.0);
float playerMoveSpeed = 0.5;

//-------------------------Network Data-------------------------
Networker* Networker::mInstance = 0;
Networker* pNetworkManager;
PacketType packetTypeReceived;
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

	//Add images to the graphics library
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
	//Default GameObject to spawn
	currentGameObjectType = GameObjectType::ROCK;
	currentGameObjectTypeString = "Rock";

	wallColour = white;

	//If server
	if (networkID == 0)
	{
		//Spawn player
		pPlayerController = new PlayerController(networkID, pGraphics, startingPlayerPos, playerMoveSpeed, PLAYER_SPRITE_IDENTIFIER);
		pNetworkManager->addGameObject(pPlayerController, networkID);
		networkID++;

		//Send it out
		pNetworkManager->sendGameObjectState(pPlayerController->getNetworkID(), PacketType::PACKET_CREATE);

		//Get client player
		pNetworkManager->receiveGameObjectState();
	}
	//If client
	else if (networkID == 1)
	{
		//Get server player
		pNetworkManager->receiveGameObjectState();

		//Spawn player
		pPlayerController = new PlayerController(networkID, pGraphics, startingPlayerPos, playerMoveSpeed, PLAYER_SPRITE_IDENTIFIER);
		pNetworkManager->addGameObject(pPlayerController, networkID);
		networkID++;

		//Send it out
		pNetworkManager->sendGameObjectState(pPlayerController->getNetworkID(), PacketType::PACKET_CREATE);
	}

	al_start_timer(timer);
}

void update()
{
	//Get mouse down input
	{
		MouseButton mouseDownInput = pInput->getMouseInput(InputMode::MouseDown);

		//Handle input
		switch (mouseDownInput)
		{
		case MouseButton::LeftMouse:
		{
			if (!bCanSpawnGameObject)
			{
				//Spawn current GameObject type
				GameObject* gameObjectToSpawn;

				switch (currentGameObjectType)
				{
				case GameObjectType::ROCK:
				{
					pair<float, float> mousePos = std::make_pair(pInput->getMouseX(), pInput->getMouseY());
					gameObjectToSpawn = dynamic_cast<GameObject*>(new Rock(networkID, pGraphics, mousePos, ROCK_SPRITE_IDENTIFIER));
					pNetworkManager->addGameObject(gameObjectToSpawn, networkID);
					pNetworkManager->sendGameObjectState(networkID, PacketType::PACKET_CREATE);
					networkID++;

					break;
				}
				case GameObjectType::WALL:
				{
					pair<float, float> mousePos = std::make_pair(pInput->getMouseX(), pInput->getMouseY());
					gameObjectToSpawn = dynamic_cast<GameObject*>(new Wall(networkID, pGraphics, mousePos, wallSizeX, wallSizeY, wallColour, wallBorderThickness));
					pNetworkManager->addGameObject(gameObjectToSpawn, networkID);
					pNetworkManager->sendGameObjectState(networkID, PacketType::PACKET_CREATE);
					networkID++;

					break;
				}

				default:
				{
					std::cout << "INVALID GAMEOBJECT TYPE! CANNOT CREATE!\n";

					break;
				}
				}

				gameObjectToSpawn = nullptr;
				bCanSpawnGameObject = true;
			}
		}

		default:
			break;
		}
	}

	//Get mouse up input
	{
		MouseButton mouseUpInput = pInput->getMouseInput(InputMode::MouseUp);
		{
			//Prevent spawning multiple gameobjects with mouse down - allow player to spawn after mouse is released again
			if (mouseUpInput == MouseButton::LeftMouse)
				bCanSpawnGameObject = false;
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
		case KeyCode::BACK:
		{
			//Players have IDs 0 and 1, DO NOT TOUCH THEM
			if (networkID > 1)
			{
				pNetworkManager->sendGameObjectState(networkID - 1, PacketType::PACKET_DELETE);
				networkID--;
			}

			break;
		}

		default:
			break;
		}
	}

	pNetworkManager->updateGameObjects();
}

void draw()
{
	//Background image
	pGraphics->drawImage(BACKGROUND_IMAGE_SPRITE_IDENTIFIER, 0, 0);

	//Draw GameObjects
	pNetworkManager->renderGameObjects();

	//Text indicator of current GameObject Type
	pGraphics->drawText(100, 50, "Current Object to Spawn: " + currentGameObjectTypeString, TextAlignment::ALIGN_LEFT);

	//Text indicators for instructions
	{
		pGraphics->drawText(pGraphics->getScreenSizeX() - 700, 50, "WASD - Move Player.", TextAlignment::ALIGN_LEFT);
		pGraphics->drawText(pGraphics->getScreenSizeX() - 700, 100, "TAB - Change GameObject to Spawn.", TextAlignment::ALIGN_LEFT);
		pGraphics->drawText(pGraphics->getScreenSizeX() - 700, 150, "Left Mouse - Spawn GameObject.", TextAlignment::ALIGN_LEFT);
		pGraphics->drawText(pGraphics->getScreenSizeX() - 700, 200, "Backspace - Delete last GameObject.", TextAlignment::ALIGN_LEFT);
		pGraphics->drawText(pGraphics->getScreenSizeX() - 700, 250, "ESC - Quit.", TextAlignment::ALIGN_LEFT);
	}

	//Render it all
	pGraphics->render();
}

void cleanup()
{
	//cleanup timer
	al_destroy_timer(timer);
	al_destroy_event_queue(eventQueue);

	//Cleanup network manager - cleans up GameObjects
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
		pNetworkManager->init(pGraphics, ROCK_SPRITE_IDENTIFIER, PLAYER_SPRITE_IDENTIFIER, playerMoveSpeed, wallColour, 60);

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

			//Server PlayerController is networkID 0
			networkID = 0;
			startingPlayerPos = STARTING_PLAYER_POSITION_SERVER;
		}
		else
		{
			//-------------------------Client code-------------------------

			//Prompt for server IP address
			std::string serverIP;
			std::cout << "Enter server IP address: \n";
			std::cin >> serverIP;

			//Prompt for port number
			std::string portNumber;
			std::cout << "Enter port number: \n";
			std::cin >> portNumber;

			bHasConnectd = pNetworkManager->connect(serverIP, portNumber);
			if (bHasConnectd)
				std::cout << "main.cpp --> client connected.\n";

			//Client PlayerController is networkID 1
			networkID = 1;
			startingPlayerPos = STARTING_PLAYER_POSITION_CLIENT;
		}

		//If the peer has connected
		if (bHasConnectd)
		{
			//Setup
			start();

			//Since 0 and 1 are used for both player controllers, start everything else at 2
			networkID = 2;

			//Loop the game
			while (bGameIsRunning)
			{
				ALLEGRO_EVENT ev;
				al_get_next_event(eventQueue, &ev);

				if (ev.type == ALLEGRO_EVENT_TIMER)
				{
					//Update call
					update();

					//Network updates - send player data
					pNetworkManager->sendGameObjectState(pPlayerController->getNetworkID(), PacketType::PACKET_UPDATE);

					//Network update - receive packets
					packetTypeReceived = pNetworkManager->receiveGameObjectState();

					//If you receive a new GameObject, increment the network ID to keep spawning in sync!
					if (packetTypeReceived == PacketType::PACKET_CREATE)
						networkID++;

					//Draw call
					draw();
				}
			}

			//Cleanup when done
			cleanup();
		}
	}

	std::cin.get();

	return 0;
}