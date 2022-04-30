#include "Sprite.h"

Sprite::Sprite(GraphicsBuffer *image, vector<int> sourceLoc, int width, int height)
{
	mpBuffer = image;
	mSourceLoc = sourceLoc;
	mWidth = width;
	mHeight = height;
}

Sprite::Sprite()
{
}


Sprite::~Sprite()
{
}

vector<int> Sprite::getSouceLoc()
{
	return mSourceLoc;
}

int Sprite::getHeight()
{
	return mHeight;
}

int Sprite::getWidth()
{
	return mWidth;
}

GraphicsBuffer Sprite::getBuffer()
{
	return *mpBuffer;
}
