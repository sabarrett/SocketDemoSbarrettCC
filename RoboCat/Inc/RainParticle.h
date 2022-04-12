#pragma once

#include "allegro.h"
#include "allegro_primitives.h"
#include "vector"

#include <string>

class RainParticle
{
public:
	RainParticle(int windowWidth, int windowHeight, float xStart, float yStart, float newRadius, float newR, float newG, float newB, float newA);
	RainParticle();
	void Read(InputMemoryBitStream& iStream);
	void Write(OutputMemoryBitStream& oStream) const;
	void Draw();
	void UpdatePos(float xChange, float yChange);

	int mWindowWidth;
	int mWindowHeight;
	std::vector<float> color;
	std::vector<float> position;
	float radius;
};

RainParticle::RainParticle(int windowWidth, int windowHeight, float xStart, float yStart, float newRadius, float newR, float newG, float newB, float newA)
{
	mWindowWidth = windowWidth;
	mWindowHeight = windowHeight;
	color.push_back(newR);
	color.push_back(newG);
	color.push_back(newB);
	color.push_back(newA);

	position.push_back(xStart);
	position.push_back(yStart);

	radius = newRadius;
}

RainParticle::RainParticle()
{
	float tempR, tempG, tempB, tempA, tempX, tempY;

	color.push_back(tempR);
	color.push_back(tempG);
	color.push_back(tempB);
	color.push_back(tempA);

	position.push_back(tempX);
	position.push_back(tempY);

	radius = float();
}

void RainParticle::Read(InputMemoryBitStream& iStream)
{
	iStream.Read(color[0]);
	iStream.Read(color[1]);
	iStream.Read(color[2]);
	iStream.Read(color[3]);

	iStream.Read(position[0]);
	iStream.Read(position[1]);

	iStream.Read(radius);
}

void RainParticle::Write(OutputMemoryBitStream& oStream) const
{
	oStream.Write(color[0]);
	oStream.Write(color[1]);
	oStream.Write(color[2]);
	oStream.Write(color[3]);

	oStream.Write(position[0]);
	oStream.Write(position[1]);

	oStream.Write(radius);
}

void RainParticle::Draw()
{
	al_draw_filled_circle(position[0], position[1], radius, al_map_rgba(color[0], color[1], color[2], color[3]));
}

void RainParticle::UpdatePos(float xChange, float yChange)
{
	position[0] += xChange;
	position[1] += yChange;
	if (position[0] > mWindowWidth) position[0] = 0;
	if (position[1] > mWindowHeight) position[1] = 0;
	if (position[0] < 0) position[0] = mWindowWidth;
	if (position[1] < 0) position[1] = mWindowHeight;
}