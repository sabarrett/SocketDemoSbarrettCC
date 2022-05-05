#pragma once
#include<iostream>
#include<vector>
#include"../common/DeanLib/include/Trackable.h"

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 3
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

class Sprite;
class GraphicsBuffer;

class Animation : public Trackable
{
public:
	Animation();
	Animation(int rows, int columns, GraphicsBuffer* buffer, bool isLooping);
	~Animation();

	void init(int numSprites, bool isInitialized, Sprite* spriteArray);
	void animInit(int rows, int columns, GraphicsBuffer* buffer, bool isLooping);
	void cleanup();

	Sprite* getCurrentSprite();
	int getSpriteWidth() { return mWidth; }
	int getSpriteHeight() { return mHeight; }

	void changeFPS(int fps);
	int getAnimationSpeed() { return mFramerate; }

	void setAnimate(bool initialized) { mIsInitialized = initialized; }

	void animationUpdate(float elapsedTime);

private:
	bool mIsInitialized;
	bool mIsLooping;

	int mCurrentSprite;
	int mNumSprites;
	int mFramerate;
	float mElapsedTime;
	float mBetweenFrames;

	int mWidth;
	int mHeight;

	Sprite* mpSpriteArray;
};