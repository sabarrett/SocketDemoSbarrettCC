#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "../common/allegro/include/allegro5/allegro.h"
#include "../common/allegro/include/allegro5/allegro_audio.h"
#include "../common/allegro/include/allegro5/allegro_acodec.h"
#include "../common/allegro/include/allegro5/allegro_image.h"
#include "../common/allegro/include/allegro5/allegro_primitives.h"
#include "../common/DeanLib/include/Trackable.h"
#include "Sprite.h"
#include "GraphicsBuffer.h"
#include "Font.h"
#include "Color.h"

const int DISP_WIDTH = 800;
const int DISP_HEIGHT = 600;

class GraphicsSystem : public Trackable
{
public:
	GraphicsSystem(int width = DISP_WIDTH, int height = DISP_HEIGHT);
	~GraphicsSystem();

	void init(); // This will also initialize the rest of allegro, make sure to use it before using any allegro functions.
	void cleanUp();

	int getHeight();
	int getWidth();
	ALLEGRO_BITMAP* getBackBuffer();

	void flip(); // Flips using al_flip. If not using allegro, change this.

	void draw(int xLoc, int yLoc, Sprite* sprite, float scale = 1.0f); // Draws a sprite at a specified location and scale.
	void draw(GraphicsBuffer targetBuffer, int xLoc, int yLoc, Sprite sprite, float scale = 1.0f); // Draws a sprite on a specified GraphicsBuffer and scale

	void writeText(int xLoc, int yLoc, Font font, Color color, std::string text);
	void writeText(GraphicsBuffer targetBuffer, int xLoc, int yLoc, Font font, Color color, std::string text);

	void saveBuffer(GraphicsBuffer targetBuffer, std::string filename); // Saves an image file of the given GraphicsBuffer
	void saveBuffer(ALLEGRO_BITMAP* targetBuffer, std::string filename); // Saves an image file of the given ALLEGRO_BITMAP, this is so the display doesn't first need to be converted to a graphics buffer to save

private:
	ALLEGRO_DISPLAY* mpDisplay;
	int mDisplayWidth;
	int mDisplayHeight;

};