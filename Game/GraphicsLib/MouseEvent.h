#pragma once
#include "Event.h"
#include <Vector2D.h>

class MouseEvent : public Event
{
public:
	MouseEvent(const int mouseButton, int x, int y);
	~MouseEvent();

	int getState() const { return mMouseButton; };
	int getX() const { return mX; };
	int getY() const { return mY; };

private:
	int mMouseButton;
	int mX, mY;
};