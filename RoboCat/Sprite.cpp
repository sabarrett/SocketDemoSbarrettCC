#include"Sprite.h"
#include"GraphicsBuffer.h"

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 2
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

Sprite::Sprite()
{
	mHeight = 0;
	mWidth = 0;
	mLocX = 0;
	mLocY = 0;
	mpGraphicsBuffer = nullptr;
}

Sprite::Sprite(GraphicsBuffer* graphicsBuffer, int locX, int locY, int height, int width)
{
	mHeight = height;
	mWidth = width;
	mLocX = locX;
	mLocY = locY;
	mpGraphicsBuffer = graphicsBuffer;
}

Sprite::~Sprite()
{
}

void Sprite::spriteInit(int width, int height, int locX, int locY, GraphicsBuffer* buffer)
{
	mWidth = width;
	mHeight = height;
	mLocX = locX;
	mLocY = locY;
	mpGraphicsBuffer = buffer;
}