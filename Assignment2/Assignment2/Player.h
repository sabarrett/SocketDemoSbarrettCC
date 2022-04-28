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
	void update();
	void render();

	static Texture playerTexture;

private:
	Vec2D mPosition;
	Vec2D mSize;
	Vec2D mVelocity;
	Color mColor;
	double mAngle;
};