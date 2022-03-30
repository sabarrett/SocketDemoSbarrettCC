#pragma once

#include "Event.h"

class DeleteUnitsEvent : public Event
{
public:
	DeleteUnitsEvent(int x, int y);
	~DeleteUnitsEvent();

	int getX() const { return mX; };
	int getY() const { return mY; };

private:
	int mX, mY;
};