/*
Allegro Wrapper Functions
Written by Adel Talhouk in FA21
GraphicsLibrary.cpp
*/

#include "GraphicsLibrary.h"
#include "RoboCatPCH.h"

#include <iostream>
#include <algorithm>

//Constructor
GraphicsLibrary::GraphicsLibrary(float screenSizeX, float screenSizeY)
{
	//Setup data - screen size
	mScreenSizeX = screenSizeX;
	mScreenSizeY = screenSizeY;

	//Allegro display
	mpDisplay = nullptr;
}

//Destructor
GraphicsLibrary::~GraphicsLibrary()
{
	//Delete bitmaps
	std::vector<std::pair<std::string, ALLEGRO_BITMAP*>>::iterator iterator;
	for (iterator = mBitmapPointersVector.begin(); iterator != mBitmapPointersVector.end(); ++iterator)
	{
		al_destroy_bitmap(iterator->second);
	}
	mBitmapPointersVector.clear();

	//Clean up font
	al_destroy_font(mpFont);
	mpFont = nullptr;

	//Clean up display
	al_destroy_display(mpDisplay);
	mpDisplay = nullptr;
}

bool GraphicsLibrary::init()
{
	//Init allegro
	if (!al_init())
	{
		std::cout << "error initting Allegro\n";
		system("pause");
		return false;
	}

	//Init image addon
	if (!al_init_image_addon())
	{
		std::cout << "error initting image add-on\n";
		system("pause");
		return false;
	}

	//Setup display
	mpDisplay = al_create_display(mScreenSizeX, mScreenSizeY);

	if (mpDisplay == nullptr)
	{
		return false;
	}

	return true;
}

bool GraphicsLibrary::initText(std::string fontFilePath, int fontSize, Colour textColour)
{
	//Init font add on
	if (!al_init_font_addon())
	{
		std::cout << "error initting font add-on\n";
		system("pause");
		return false;
	}

	//Init ttf add on
	if (!al_init_ttf_addon())
	{
		std::cout << "error initting ttf add-on\n";
		system("pause");
		return false;
	}

	//Init primitives
	if (!al_init_primitives_addon())
	{
		std::cout << "error initting primitives add-on\n";
		system("pause");
		return false;
	}

	//Init font
	mpFont = al_load_ttf_font(fontFilePath.c_str(), fontSize, 0);
	mTextColour = al_map_rgba(textColour.getR(), textColour.getG(), textColour.getB(), textColour.getA());

	return true;
}

void GraphicsLibrary::render()
{
	//Flip display buffers
	al_flip_display();
}

void GraphicsLibrary::loadImage(std::string imageFilePath, std::string imageIdentifier)
{
	//Add the name of the image and the loaded bitmap to the vector of pairs
	mBitmapPointersVector.push_back(std::make_pair(imageIdentifier, al_load_bitmap(imageFilePath.c_str())));
}

void GraphicsLibrary::drawText(float posX, float posY, std::string text, TextAlignment alignment)
{
	al_draw_text(mpFont, mTextColour, posX, posY, alignment, text.c_str());
}

void GraphicsLibrary::drawRectangle(float topLeftX, float topLeftY, float bottomRightX, float bottomRightY, Colour colour, float thickness)
{
	//Set colour
	ALLEGRO_COLOR col = al_map_rgba(colour.getR(), colour.getG(), colour.getB(), colour.getA());
	al_draw_rectangle(topLeftX, topLeftY, bottomRightX, bottomRightY, col, thickness);
}

void GraphicsLibrary::drawImage(std::string imageIdentifier, float posX, float posY)
{
	//Find the image and draw if it exists
	std::vector<std::pair<std::string, ALLEGRO_BITMAP*>>::iterator iterator;

	for (iterator = mBitmapPointersVector.begin(); iterator != mBitmapPointersVector.end(); ++iterator)
	{
		if (iterator->first == imageIdentifier)
		{
			al_draw_bitmap(iterator->second, posX, posY, 0);
		}
	}
}

void GraphicsLibrary::drawScaledImage(std::string imageIdentifier, float posX, float posY, float scaleX, float scaleY)
{
	//Find the image and draw if it exists
	std::vector<std::pair<std::string, ALLEGRO_BITMAP*>>::iterator iterator;

	for (iterator = mBitmapPointersVector.begin(); iterator != mBitmapPointersVector.end(); ++iterator)
	{
		if (iterator->first == imageIdentifier)
		{
			al_draw_scaled_bitmap(iterator->second, 0, 0, al_get_bitmap_width(iterator->second), al_get_bitmap_height(iterator->second), posX, posY, al_get_bitmap_width(iterator->second) * scaleX, al_get_bitmap_height(iterator->second) * scaleY, 0);
		}
	}
}

void GraphicsLibrary::drawTintedImage(std::string imageIdentifier, float posX, float posY, Colour col)
{
	//Find the image and draw if it exists
	std::vector<std::pair<std::string, ALLEGRO_BITMAP*>>::iterator iterator;

	//Set colour
	ALLEGRO_COLOR colour = al_map_rgba(col.getR(), col.getG(), col.getB(), col.getA());

	for (iterator = mBitmapPointersVector.begin(); iterator != mBitmapPointersVector.end(); ++iterator)
	{
		if (iterator->first == imageIdentifier)
		{
			al_draw_tinted_bitmap(iterator->second, colour, posX, posY, 0);
		}
	}
}