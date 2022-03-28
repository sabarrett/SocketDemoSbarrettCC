
#include "RoboCatPCH.h"
#include "InputSystem.h"
#include "GraphicsLibrary.h"	//This itself includes Colour
#include "GameObject.h"
#include "Rock.h"
#include "Wall.h"

//-------------------------Graphics Data-------------------------
GraphicsLibrary* pGraphics;
float screenSizeX = 1600.0;
float screenSizeY = 900.0;

//-------------------------Input Data-------------------------
InputSystem* pInput;

//-------------------------Assets-------------------------
const std::string ASSET_PATH = "Assets\\";
const std::string BACKGROUND_IMAGE_FILE = "Background_Image.jpg";
const std::string ARIAL_FONT_FILE = "ARIBL0.ttf";
const int FONT_SIZE = 32;

//-------------------------Colours-------------------------
Colour white(255, 255, 255, 255);

//-------------------------Asset Identifiers-------------------------
const std::string backgroundImageSprite = "background_image";

//-------------------------Game Data-------------------------
bool bGameIsRunning = true;
std::vector<GameObject*> gameObjectsVec;

float wallSizeX = 150;
float wallSizeY = 15;

//-------------------------GameObject Data-------------------------
GameObjectType currentGameObjectType;
std::string currentGameObjectTypeString;
int gameObjectID = 0;

//-------------------------Network Data-------------------------
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
	pGraphics->loadImage(ASSET_PATH + BACKGROUND_IMAGE_FILE, backgroundImageSprite);

	//Setup the input system
	pInput = new InputSystem();
	if (bSuccessfulInit)
		bSuccessfulInit = pInput->init(pGraphics);

	//Init socket utils
	SocketUtil::StaticInit();

	//Init and return if it succeeded or not
	return bSuccessfulInit;
}

void start()
{
	//Default GameObject to spawn
	currentGameObjectType = GameObjectType::ROCK;
	currentGameObjectTypeString = "Rock";
}

void update()
{
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
				gameObjectToSpawn = dynamic_cast<GameObject*>(new Rock(gameObjectID, networkID, mousePos));

				break;
			}
			case GameObjectType::WALL:
			{
				pair<float, float> mousePos = std::make_pair(pInput->getMouseX(), pInput->getMouseY());
				gameObjectToSpawn = dynamic_cast<GameObject*>(new Wall(gameObjectID, networkID, mousePos, wallSizeX, wallSizeY));

				break;
			}

			default:
			{
				std::cout << "INVALID GAMEOBJECT TYPE! CANNOT CREATE!\n";

				break;
			}
			}

			gameObjectsVec.push_back(gameObjectToSpawn);
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
		KeyCode keyCode = pInput->getKeyboardInput();

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
	for (unsigned int i = 0; i < gameObjectsVec.size(); i++)
	{
		gameObjectsVec[i]->update();
	}
}

void draw()
{
	//Background image
	pGraphics->drawImage(backgroundImageSprite, 0, 0);

	//Draw GameObjects
	for (unsigned int i = 0; i < gameObjectsVec.size(); i++)
	{
		gameObjectsVec[i]->draw();
	}

	//Text indicator of current GameObject Type
	pGraphics->drawText(100, 50, "Current Object to Spawn: " + currentGameObjectTypeString, TextAlignment::ALIGN_LEFT);

	//Text indicators for instructions
	{
		pGraphics->drawText(pGraphics->getScreenSizeX() - 700, 50, "TAB - Change GameObject to Spawn.", TextAlignment::ALIGN_LEFT);
		pGraphics->drawText(pGraphics->getScreenSizeX() - 700, 100, "Left Mouse - Spawn GameObject.", TextAlignment::ALIGN_LEFT);
		pGraphics->drawText(pGraphics->getScreenSizeX() - 700, 150, "ESC - Quit.", TextAlignment::ALIGN_LEFT);
	}

	//Render it all
	pGraphics->render();
}

void cleanup()
{
	//Cleanup GameObjects
	for (unsigned int i = 0; i < gameObjectsVec.size(); i++)
	{
		delete gameObjectsVec[i];
		gameObjectsVec[i] = nullptr;
	}
	
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
		//Setup
		start();

		//Loop the game
		while (bGameIsRunning)
		{
			//Update call
			update();

			//Draw call
			draw();
		}

		//Cleanup when done
		cleanup();
		SocketUtil::CleanUp();
	}

	return 0;
}
