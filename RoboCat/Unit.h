#pragma once
#include<iostream>
#include<vector>
#include<allegro5/allegro.h>
#include"../common/DeanLib/include/Trackable.h"
#include"Animation.h"

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 3
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

class Animation;
class GraphicsBuffer;
class GraphicsSystem;
class System;

class Unit : public Trackable
{
public:
	Unit();
	Unit(int locX, int locY, Animation* animate);
	~Unit();

	void init(Animation* animate, int locX, int locY);

	int getLocX() { return mLocX; }
	int getLocY() { return mLocY; }
	int getSpriteWidth() { return mpAnimate->getSpriteWidth(); }
	int getSpriteHeight() { return mpAnimate->getSpriteHeight(); }
	Animation* getAnimation() { return mpAnimate; }

	void locationUpdate(float elapsedTime);
	void drawSprite(System* system);
	void addAnimation(Animation* toAdd);
	void setAnimation(Animation* toAdd);
	void setPosition(int locX, int locY);
	void setAnimationSpeed(int speed);
	void toggleAnimations(bool animate);

	void cleanup();
private:
	int mLocX;
	int mLocY;

	std::vector<Animation*> mpAnimationList;

	Animation* mpAnimate;
	bool mIsAnimated;
	float mScale = 1;
};