
#include "RoboCatPCH.h"
#include "InputSystem.h"
#include "GraphicsLibrary.h"	//This itself includes Colour

//-------------------------Graphics Data-------------------------
GraphicsLibrary* pGraphics;
float screenSizeX = 1600.0;
float screenSizeY = 900.0;
bool bShouldRedraw = true;

//-------------------------Input Data-------------------------
InputSystem* pInput;

//-------------------------Assets-------------------------
const std::string ASSET_PATH = "..\\Assets\\";
const std::string BACKGROUND_IMAGE_FILE = "Background_Image.jpg";

//-------------------------Asset Identifiers-------------------------
const std::string backgroundImageSprite = "background_image";

//-------------------------Game Data-------------------------
bool bGameIsRunning = true;

bool init()
{
	bool bSuccessfulInit = false;

	//Setup the graphical window
	pGraphics = new GraphicsLibrary(screenSizeX, screenSizeY);
	bSuccessfulInit = pGraphics->init(ASSET_PATH + BACKGROUND_IMAGE_FILE);

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


			break;
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

		case KeyCode::Esc:
		{
			//Quit game
			bGameIsRunning = false;
			bShouldRedraw = false;	//Skip over draw call, just end the "game"
			return;
			break;
		}

		default:
			break;
		}
	}
}

void draw()
{
	//Background image
	pGraphics->drawImage(backgroundImageSprite, screenSizeX * 0.5, screenSizeY * 0.5);

	//TO-DO: Draw more stuff here


	//Render it all
	pGraphics->render();

	//Do not re-draw each frame (efficiency)
	bShouldRedraw = false;
}

void cleanup()
{
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

			//If it should redraw - to save framerate
			if (bShouldRedraw)
			{
				//Draw call
				draw();
			}
		}

		//Cleanup when done
		cleanup();
		SocketUtil::CleanUp();
	}

	return 0;
}
