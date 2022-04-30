#include <allegro5/allegro.h>
#include "Colour.h"
#include "GraphicsLibrary.h"
#include "InputSystem.h"
#include <iostream>


//-------------------------Graphics Data-------------------------

GraphicsLibrary * pGraphics;

float screenSizeX = 1600.0;

float screenSizeY = 900.0;

//-------------------------Input Data-------------------------

InputSystem* pInput;

//-------------------------Assets-------------------------

const std::string ASSET_PATH = "..\\Assets/";

const std::string BACKGROUND_IMAGE_FILE = "Background_Image.jpg";

//-------------------------Asset Identifiers-------------------------

const std::string backgroundImageSprite = "background_image_image";

bool init()

{

	bool bSuccessfulInit = false;

	//Setup the graphical window
	pGraphics = new GraphicsLibrary(screenSizeX, screenSizeY);
	bSuccessfulInit = pGraphics->init(ASSET_PATH + BACKGROUND_IMAGE_FILE);

	//Add images to the graphcis library
	pGraphics->loadImage(ASSET_PATH + BACKGROUND_IMAGE_FILE, backgroundImageSprite);

	pGraphics->drawImage(backgroundImageSprite, screenSizeX/2, screenSizeY/2);

	//Setup the input system
	pInput = new InputSystem();
	if (bSuccessfulInit)
		bSuccessfulInit = pInput->init(pGraphics);

	//Init and return if it succeeded or not
	return bSuccessfulInit;
}

int main()
{
	bool t = init();

	std::cin.get();

	return 0;
}