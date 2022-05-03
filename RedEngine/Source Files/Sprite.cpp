#include "Sprite.h"
#include "GraphicsBuffer.h"

Sprite::Sprite(GraphicsBuffer* gb, Vector2D loc, Vector2D size, float scale)
{
	mpGraphicsBuffer = gb;
	mLoc = loc;
	mSize = size;
	mScale = scale;
}

Sprite::~Sprite()
{
	mpGraphicsBuffer = nullptr;
}