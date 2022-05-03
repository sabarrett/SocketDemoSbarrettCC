#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include "Globals.h"
#include "Texture.h"
#include "Vec2D.h"
#include "Color.h"

class Bullet
{
public:
	Bullet();
	void setColor(Color newColor);
	void update();
	void render();
	void setVelocity(Vec2D newVec2D);
	void setPosition(Vec2D newPos);
	Vec2D getPosition();

	void WriteBits(OutputMemoryBitStream& inStream);
	void ReadBits(InputMemoryBitStream& inStream);

	static Texture bulletTexture;

private:
	Vec2D mPosition;
	Vec2D mVelocity;
	Vec2D mSize;
	Color mColor;
};