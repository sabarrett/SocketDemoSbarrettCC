#pragma once
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "Vec2D.h"
#include "Globals.h"

class Texture
{
public:
	Texture();
	~Texture();

	bool loadFromFile(std::string path);
	void free();
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	Vec2D getSize();
	SDL_Texture* getTexture();
private:
	SDL_Texture* mTexture;
	Vec2D mSize;
};