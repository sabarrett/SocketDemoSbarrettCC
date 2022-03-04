#pragma once

#include "raylib-cpp.hpp"
#include "Trackable.h"

const int MAX_COLOR_VALUE = 255;

class RColor : public Trackable
{

public:
	friend class GraphicsSystem;

	RColor();
	~RColor();
	RColor(int, int, int, int);
	RColor(float, float, float, float);

private:
	int mR, mG, mB, mA;

	raylib::Color getRayColor();

};
