#pragma once

/*
Allegro Wrapper Functions
Written by Adel Talhouk in FA21
GraphicsLibrary.h

	File information:
	This file contains function abstractions from Allegro 5, wrapped up in my Graphics Library. This will
	be used to render images and text to the screen.

	Source I am consulting: Allegro 5.0.10 Manual - http://cdn.allegro.cc/file/library/allegro/5.0.10/allegro-5.0.10-manual.pdf
*/

#include <string>
#include <vector>

#include "Colour.h"

//https://github.com/liballeg/allegro_wiki/wiki/Allegro-in-Visual-Studio#using-nuget-within-visual-studio
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

class GraphicsLibrary
{
	//-------------------------Private data-------------------------

	//Screen data
	float mScreenSizeX;
	float mScreenSizeY;

	//Allegro display
	ALLEGRO_DISPLAY* mpDisplay;

	//Other images to draw
	std::vector<std::pair<std::string, ALLEGRO_BITMAP*>> mBitmapPointersVector;

	friend class InputSystem;

	//-------------------------Public data-------------------------
public:

	//Constructor(s)
	GraphicsLibrary(float screenSizeX, float screenSizeY);

	//Destructor
	~GraphicsLibrary();

	//Accessor(s)
	float getScreenSizeX() { return mScreenSizeX; };
	float getScreenSizeY() { return mScreenSizeY; };

	//Mutator(s)

	//Functions
	bool init(std::string backgroundFilePath);
	void render();
	void loadImage(std::string imageFilePath, std::string imageIdentifier);

	//Drawing functions
	void drawImage(std::string imageIdentifier, float posX, float posY);
	void drawTintedImage(std::string imageIdentifier, float posX, float posY, Colour col);
};