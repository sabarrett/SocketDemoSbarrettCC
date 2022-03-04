
#include "RoboCatPCH.h"
#include "InputSystem.h"
#include "GraphicsLibrary.h"	//This itself includes Colour

//-------------------------Graphics Data-------------------------
GraphicsLibrary* pGraphics;
float screenSizeX = 1600.0;
float screenSizeY = 900.0;

//-------------------------Input Data-------------------------
InputSystem* pInput;

//-------------------------Assets-------------------------
const std::string ASSET_PATH = "..\\Assets\\";
const std::string BACKGROUND_IMAGE_FILE = "Background_Image.jpg";

//-------------------------Asset Identifiers-------------------------
const std::string backgroundImageSprite = "background_image";

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

	}

	SocketUtil::CleanUp();

	return 0;
}
