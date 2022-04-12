#pragma once

#include "allegro.h"
#include "allegro_primitives.h"
#include <string>
#include <vector>

class CircleClass
{
public:
	CircleClass(int windowWidth, int windowHeight, int xStart, int yStart, float newRadius);
	CircleClass();
	void Read(InputMemoryBitStream& iStream);
	void Write(OutputMemoryBitStream& oStream) const;
	void Draw();
	void UpdatePos(int xChange, int yChange);

	int mWindowWidth;
	int mWindowHeight;
	std::vector<int> position;
	float radius;
};

void CircleClass::Read(InputMemoryBitStream& iStream)
{
	iStream.Read(position[0]);
	iStream.Read(position[1]);
	iStream.Read(radius);
}

void CircleClass::Write(OutputMemoryBitStream& oStream) const
{
	oStream.Write(position[0]);
	oStream.Write(position[1]);
	oStream.Write(radius);
}

void CircleClass::Draw()
{
	al_draw_filled_circle(position[0], position[1], radius, al_map_rgb(0, 255, 0));
}

CircleClass::CircleClass(int windowWidth, int windowHeight, int xStart, int yStart, float newRadius)
{
	mWindowWidth = windowWidth;
	mWindowHeight = windowHeight;
	position.push_back(xStart);
	position.push_back(yStart);
	radius = newRadius;
}

CircleClass::CircleClass()
{
	int xTemp = 0;
	int yTemp = 0;
	position.push_back(xTemp);
	position.push_back(yTemp);
	radius = 10;
}

void CircleClass::UpdatePos(int xChange, int yChange)
{
	position[0] += xChange;
	position[1] += yChange;
	if (position[0] > mWindowWidth) position[0] = 0;
	if (position[1] > mWindowHeight) position[1] = 0;
	if (position[0] < 0) position[0] = mWindowWidth;
	if (position[1] < 0) position[1] = mWindowHeight;
}