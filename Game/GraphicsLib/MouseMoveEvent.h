#pragma once

#include <Vector2D.h>
#include "Event.h"

class MouseMoveEvent : public Event
{
public:
	MouseMoveEvent(const Vector2D& loc);
	~MouseMoveEvent();

	Vector2D getLocation() const { return mLoc; };

private:
	Vector2D mLoc;
};