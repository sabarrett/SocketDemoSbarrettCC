#include "LTexture.h"

LTexture::LTexture()
{
	mTexture = NULL;
	mRenderer = NULL;
	mSize.x = 0;
	mSize.y = 0;
	mPosition.x = 0;
	mPosition.y = 0;
	mVelocity.x = 0;
	mVelocity.y = 0;
	mAngle = 0.0;
}

LTexture::LTexture(SDL_Renderer** renderer)
{
	mTexture = NULL;
	mRenderer = renderer;
	mSize.x = 0;
	mSize.y = 0;
	mPosition.x = 0;
	mPosition.y = 0;
	mVelocity.x = 0;
	mVelocity.y = 0;
	mAngle = 0.0;
}

LTexture::~LTexture()
{
	free();
}

bool LTexture::loadFromFile(std::string path)
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
		newTexture = SDL_CreateTextureFromSurface(*mRenderer, loadedSurface);
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

void LTexture::setRenderer(SDL_Renderer** renderer)
{
	mRenderer = renderer;
}

void LTexture::free()
{
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
	}
}

void LTexture::render()
{
	// desination rectangle to render to
	SDL_Rect renderQuad = { mPosition.x, mPosition.y, mSize.x, mSize.y };
	SDL_RenderCopyEx(*mRenderer, mTexture, NULL, &renderQuad, mAngle, NULL, SDL_FLIP_NONE);
}

void LTexture::update()
{
	mVelocity.x *= 0.998;
	mVelocity.y *= 0.998;

	if (abs(mVelocity.x) < 0.001 && abs(mVelocity.y) < 0.001)
	{
		mVelocity.x = 0.0;
		mVelocity.y = 0.0;
	}

	mPosition.x += mVelocity.x;
	mPosition.y += mVelocity.y;
}

void LTexture::setPos(Vec2D newPosition)
{
	mPosition.x = newPosition.x;
	mPosition.y = newPosition.y;
}

void LTexture::setAngle(double degrees)
{
	mAngle = degrees;
}

void LTexture::faceTowards(Vec2D position)
{
	// https://gamefromscratch.com/gamedev-math-recipes-rotating-to-face-a-point/
	int centerX = mPosition.x + (mSize.x / 2);
	int centerY = mPosition.y + (mSize.y / 2);
	double angle = atan2(position.y - centerY, position.x - centerX);
	angle = angle * (180 / M_PI);
	setAngle(angle);
}

void LTexture::setVelocity(Vec2D newVelocity)
{
	mVelocity.x = newVelocity.x;
	mVelocity.y = newVelocity.y;
}

Vec2D LTexture::getSize()
{
	return mSize;
}

Vec2D LTexture::getPosition()
{
	return mPosition;
}

int LTexture::getAngle()
{
	return mAngle;
}


