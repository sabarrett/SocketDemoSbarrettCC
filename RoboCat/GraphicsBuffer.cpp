#include"GraphicsBuffer.h"
#include"Color.h"

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 2
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

GraphicsBuffer::GraphicsBuffer()
{
	mWidth = 800;
	mHeight = 600;
	mpGraphicsBuffer = al_create_bitmap(mWidth, mHeight);
	mIsInited = false;
}

GraphicsBuffer::GraphicsBuffer(string filename)
{
	if (mpGraphicsBuffer == nullptr)
	{
		mIsInited = true;
		mpGraphicsBuffer = al_load_bitmap(filename.c_str());
		assert(mpGraphicsBuffer);
		mWidth = al_get_bitmap_width(mpGraphicsBuffer);
		mHeight = al_get_bitmap_height(mpGraphicsBuffer);
	}
	
}

GraphicsBuffer::GraphicsBuffer(int height, int width)
{
	mpGraphicsBuffer = al_create_bitmap(width, height);
}

GraphicsBuffer::GraphicsBuffer(int width, int height, Color* color)
{
	mWidth = width;
	mHeight = height;

	ALLEGRO_BITMAP* pOldBuffer = al_get_target_bitmap();
	mIsInited = true;
	mpGraphicsBuffer = al_create_bitmap(width, height);
	al_set_target_bitmap(mpGraphicsBuffer);
	assert(mpGraphicsBuffer);
	al_clear_to_color(color->getColor());
	al_set_target_bitmap(pOldBuffer);
}

GraphicsBuffer::~GraphicsBuffer()
{
}