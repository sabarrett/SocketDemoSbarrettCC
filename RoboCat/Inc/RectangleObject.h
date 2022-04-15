#pragma once

#include "allegro.h"
#include "allegro_primitives.h"
#include <string>

class RectangleObject
{
public:
	string mName;
	int mWindowWidth;
	int mWindowHeight;
	int width;
	int height;
	int xPos;
	int yPos;
	void Read(InputMemoryBitStream& stream);
	void Write(OutputMemoryBitStream& stream) const;
	RectangleObject(string name, int windowWidth, int windowHeight, int newWidth, int newHeight, int startPosX, int startPosY);
	RectangleObject();
	void UpdatePos(int xChange, int yChange);
	void Draw();
	//bool operator==(const RectangleObject& other);

};
//bool RectangleObject::operator==(const RectangleObject& other)
//{
//	return mName == other.mName;
//}
void RectangleObject::Read(InputMemoryBitStream& stream)
{
	stream.Read(mName);
	stream.Read(width);
	stream.Read(height);
	stream.Read(xPos);
	stream.Read(yPos);
}

void RectangleObject::Write(OutputMemoryBitStream& stream) const
{
	stream.Write(mName);
	stream.Write(width);
	stream.Write(height);
	stream.Write(xPos);
	stream.Write(yPos);
}

RectangleObject::RectangleObject(string name, int windowWidth, int windowHeight, int newWidth, int newHeight, int startPosX, int startPosY)
{
	mName = name;
	mWindowWidth = windowWidth;
	mWindowHeight = windowHeight;
	width = newWidth;
	height = newHeight;
	xPos = startPosX;
	yPos = startPosY;
}
RectangleObject::RectangleObject()
{
	mName = "";
	mWindowWidth = 100;
	mWindowHeight = 100;
	width = 10;
	height = 10;
	xPos = rand() % 100 + 1;
	yPos = rand() % 100 + 1;
}

void RectangleObject::UpdatePos(int xChange, int yChange)
{
	xPos += xChange;
	yPos += yChange;
	if (xPos > mWindowWidth) xPos = 0;
	if (yPos > mWindowHeight) yPos = 0;
	if (xPos < 0) xPos = mWindowWidth;
	if (yPos < 0) yPos = mWindowHeight;
}

void RectangleObject::Draw()
{
	al_draw_filled_rectangle(xPos, yPos, xPos + width, yPos + height, al_map_rgb(255, 0, 0));
}