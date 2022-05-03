#include "RoboCatPCH.h"

Texture::Texture()
{
	mTexture = NULL;
	mSize.x = 0;
	mSize.y = 0;
}

Texture::~Texture()
{
	free();
}

bool Texture::loadFromFile(std::string path)
{
	free();

	SDL_Texture* newTexture = NULL;

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Failure loading image %s.  Error: %s\n", path.c_str(), SDL_GetError());
	}
	else
	{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0, 0xFF));
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Failure creating texture %s.  Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			mSize = Vec2D(loadedSurface->w, loadedSurface->h);
		}
		SDL_FreeSurface(loadedSurface);
	}
	mTexture = newTexture;
	return newTexture != NULL;
}

void Texture::free()
{
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
	}
}

void Texture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Rect renderQuad = { x, y, mSize.x, mSize.y };

	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

Vec2D Texture::getSize()
{
	return mSize;
}

SDL_Texture* Texture::getTexture()
{
	return mTexture;
}
