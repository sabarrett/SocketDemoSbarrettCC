#pragma once

#include <string>

#include "GraphicsLibrary.h"
#include "GameController.h"

class Boulder
{
public:
	
	Boulder();
	~Boulder();
	void spawn(int posX, int posY);
	void drop();

private:
	
	std::string mImageFileBoulder;
	int mXLoc;
	int mYLoc;
	int mBoulderSpeed;

};

class Bees
{
public:

	Bees();
	~Bees();
	void spawn(KeyCode direction);
	void move();

private:

	std::string mImageFileBee;
	KeyCode mDirection;
	int mBeeQuantity;
	int mBeeSpeed;

};

class Bubble
{
public:

	Bubble();
	~Bubble();
	void spawn(std::string playerID);
	void pop();
	void move();

private:

	std::string mImageFileBubble;
	Colour mBubbleColor;
	int mBubbleSpeed;

};