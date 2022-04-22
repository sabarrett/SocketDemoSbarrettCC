#pragma once

#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
#include "Vec2D.h"

class LTexture
{
public:
	LTexture();
	LTexture(SDL_Renderer** renderer);
	~LTexture();

	bool loadFromFile(std::string path);
	void setRenderer(SDL_Renderer** renderer);
	void free();
	void render();
	void update();
	void setPos(Vec2D newPosition);
	void setAngle(double degrees);
	void faceTowards(Vec2D position);
	void setVelocity(Vec2D newVelocity);
	Vec2D getSize();
	Vec2D getPosition();
	int getAngle();
private:
	SDL_Texture* mTexture;
	SDL_Renderer** mRenderer;
	Vec2D mSize;
	Vec2D mPosition;
	Vec2D mVelocity;
	int mAngle;
};