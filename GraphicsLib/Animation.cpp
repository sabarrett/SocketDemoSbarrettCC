#include"Animation.h"
#include"GraphicsBuffer.h"
#include"Sprite.h"

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 3
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

Animation::Animation()
{
	mIsInitialized = false;
	mIsLooping = false;

	mCurrentSprite = 16;
	mNumSprites = 0;
	mFramerate = 60;
	mElapsedTime = 0.0;
	mBetweenFrames = 0.0;

	mWidth = 0;
	mHeight = 0;

	mpSpriteArray = nullptr;
}

void Animation::animInit( int rows, int columns, GraphicsBuffer* buffer, bool isLooping )
{
	mIsInitialized = true;

	mWidth = buffer->getWidth() / columns;
	mHeight = buffer->getHeight() / rows;
	mNumSprites = rows * columns;
	mIsLooping = isLooping;

	mpSpriteArray = new Sprite[mNumSprites]();
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			mpSpriteArray[j + (i * columns)].spriteInit( mWidth, mHeight, (mWidth * j), (mHeight * i), buffer );
		}
	}

	mCurrentSprite = 0;
	mElapsedTime = 0.0;
	mFramerate = 60;
	mBetweenFrames = 1 / mFramerate;
}

Animation::Animation(int rows, int columns, GraphicsBuffer* buffer, bool isLooping)
{
	animInit(rows, columns, buffer, isLooping);
}

Animation::~Animation()
{
	cleanup();
}

void Animation::init(int numSprites, bool isInitialized, Sprite* spriteArray)
{
	mNumSprites = numSprites;
	mIsInitialized = isInitialized;
	mpSpriteArray = spriteArray;

	if (mIsInitialized == false)
	{
		mIsInitialized = true;
	}
}


void Animation::cleanup()
{
	if (mIsInitialized == true)
	{
		delete [] mpSpriteArray;
		mpSpriteArray = nullptr;

		mIsInitialized = false;
		mIsLooping = false;
	}
}

Sprite* Animation::getCurrentSprite()
{
	return &mpSpriteArray[mCurrentSprite];
}

void Animation::changeFPS(int fps)
{
	mBetweenFrames = 1.0 / fps;
	mFramerate = fps;
}

void Animation::animationUpdate(float elapsedTime)
{
	mElapsedTime += elapsedTime;

	if (mElapsedTime > mBetweenFrames)
	{
		mElapsedTime -= mBetweenFrames;
		mCurrentSprite++;

		// loop back to first sprite
		if (mCurrentSprite == mNumSprites)
		{
			mCurrentSprite = 0;
		}
	}
}