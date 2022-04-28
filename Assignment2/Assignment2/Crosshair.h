#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "Globals.h"
#include "Texture.h"
#include "Vec2D.h"
#include "Color.h"

class Crosshair
{
public:
	Crosshair();
	void handleEvent(SDL_Event& e);
	void setColor(Color newColor);
	void update();
	void render();

	static Texture crosshairTexture;

private:
	Vec2D mPosition;
	Vec2D mSize;
	Color mColor;
};