#pragma once
#include<iostream>
#include<string>
#include<allegro5/allegro.h>
#include"../common/DeanLib/include/Trackable.h"

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 2
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

class GraphicsBuffer;

class Sprite : public Trackable
{
public:
	Sprite();
	Sprite(GraphicsBuffer* graphicsBuffer, int locX, int locY, int height, int width);
	~Sprite();

	int getHeight() { return mHeight; }
	int getWidth() { return mWidth; }
	int getLocX() { return mLocX; }
	int getLocY() { return mLocY; }
	void spriteInit(int width, int height, int locX, int locY, GraphicsBuffer* buffer);
	GraphicsBuffer* getGraphicsBuffer() { return mpGraphicsBuffer; }

private:

	int mHeight;
	int mWidth;
	int mLocX;
	int mLocY;

	GraphicsBuffer* mpGraphicsBuffer;
};