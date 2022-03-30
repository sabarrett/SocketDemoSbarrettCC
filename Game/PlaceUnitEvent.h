#pragma once

#include <Vector2D.h>
#include "Event.h"

class PlaceUnitEvent : public Event
{
public:
public:
	PlaceUnitEvent(int num);
	~PlaceUnitEvent();

	int getX() const { return mX; };
	int getY() const { return mY; };

private:
	int mX, mY, type;
};