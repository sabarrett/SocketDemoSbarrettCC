#pragma once

#include <allegro5/allegro.h>

#include <Trackable.h>

class GraphicsSystem;

class Color :public Trackable
{
	friend GraphicsSystem;
public:
	Color(int r=255, int g=255, int b=255, int a = 255);
	Color(float r, float g, float b, float a = 1.0f);

	int getR() const { return mR; };
	int getG() const { return mG; };
	int getB() const { return mB; };
	int getA() const { return mA; };

private:
	int mR = 255;
	int mG = 255;
	int mB = 255;
	int mA = 255;
};
