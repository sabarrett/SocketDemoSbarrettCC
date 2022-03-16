#pragma once

#include <Vector2D.h>
#include "Event.h"

class MoveEvent : public Event
{
public:
	MoveEvent( const Vector2D& loc );
	~MoveEvent();

	Vector2D getLocation() const { return mLoc; };

private:
	Vector2D mLoc;
};