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
using namespace std;

class Color;

class GraphicsBuffer : public Trackable
{
public:
	GraphicsBuffer();
	GraphicsBuffer(string filename);
	GraphicsBuffer(int height, int width);
	GraphicsBuffer(int width, int height, Color* color);
	GraphicsBuffer( GraphicsBuffer& ) = delete; // prevents the creation of a copy constructor so people can't pass this class by value
	~GraphicsBuffer();

	int getWidth() { return mWidth; }
	int getHeight() { return mHeight; }
	ALLEGRO_BITMAP* getGraphicsBuffer() { return mpGraphicsBuffer; }
private:
	int mHeight;
	int mWidth;
	ALLEGRO_BITMAP* mpGraphicsBuffer;
	bool mIsInited;
};