# allegro_wrapper_functions
Some Allegro 5 wrapper functions for C++

I wrote some code for my Computer Architecture final project in FA21 that allowed me to use abstracted Allegro 5 functions. Feel free to use this code, but please keep the block comment in each file :)

EXAMPLE INIT FUNCTION IN MAIN.CPP:

```cpp
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
const std::string backgroundImageSprite = "background_image_image";

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

	//Init and return if it succeeded or not
	return bSuccessfulInit;
}
```
