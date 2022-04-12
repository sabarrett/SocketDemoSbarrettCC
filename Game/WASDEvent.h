#pragma once
#include "Event.h"
#include <Vector2D.h>
class WASDEvent :
    public Event
{
	public:
		WASDEvent(Vector2D newDir);
		~WASDEvent();

		Vector2D dir;
};

