#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <string>


using namespace std;

class GraphicsBuffer
{
public:
	friend class GraphicsSystem;
	friend class GraphicsBufferManager;
	~GraphicsBuffer();

private:
	ALLEGRO_BITMAP* mpBitmap;

	GraphicsBuffer();
	GraphicsBuffer(string ASSET_PATH); //Create a bitmap of an image

	int getHeight();
	int getWidth();

};