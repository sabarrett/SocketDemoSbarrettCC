#pragma once

#include <allegro5/allegro.h>

#include "Sprite.h"


#include <string>

using namespace std;

class GraphicsSystem
{
public:
	GraphicsSystem();
	~GraphicsSystem();

	void init(int width, int height); //intialize all necassary addons
	void cleanup();

	int getHeight();
	int getWidth();

	void flip();

	void draw(int x, int y, Sprite sprite, float scale); //Draw sprite to graphics system
	void draw(GraphicsBuffer* backBuffer, int x, int y, Sprite sprite, float scale); //Draw sprite to given GraphicsBuffer
	void draw(GraphicsBuffer* backBuffer, int x, int y, float scale);
	void saveBuffer(string filename); //Save the buffer
	void saveBuffer(GraphicsBuffer* backBufferToSave, string filename); //Save the given buffer

private:
	ALLEGRO_DISPLAY* mpDisplay;
};