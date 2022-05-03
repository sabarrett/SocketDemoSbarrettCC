#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "Globals.h"
#include "Texture.h"
#include "Vec2D.h"
#include "Color.h"

class Player
{
public:
	Player();
	void handleEvent(SDL_Event& e);
	void faceTowards(Vec2D pos);
	void setColor(Color newColor);
	Vec2D getPosition();
	Vec2D getSize();
	Vec2D getVelocity();
	void update();
	void render();

	void WriteBits(OutputMemoryBitStream& inStream);
	void ReadBits(InputMemoryBitStream& inStream);

	static Texture playerTexture;

private:
	Vec2D mPosition;
	Vec2D mSize;
	Vec2D mVelocity;
	Color mColor;
	double mAngle;
};