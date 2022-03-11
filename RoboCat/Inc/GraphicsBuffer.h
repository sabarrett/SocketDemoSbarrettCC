#pragma once
#include <allegro5/allegro.h>
#include <iostream>

class GraphicsBuffer
{
public:
	
	GraphicsBuffer();
	GraphicsBuffer(std::string fileName);
	GraphicsBuffer(int width, int height);
	
	~GraphicsBuffer();

	int getHeight() { return al_get_bitmap_height(mpBitmap); }
	int getWidth() { return al_get_bitmap_width(mpBitmap); }

private:
	ALLEGRO_BITMAP* mpBitmap = nullptr;
	bool mBufferTaken = false;

	ALLEGRO_BITMAP* getBitmap() const { return mpBitmap; }
	GraphicsBuffer(ALLEGRO_BITMAP* setBitmap);
};
