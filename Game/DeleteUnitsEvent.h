#pragma once

#include "Event.h"

class DeleteUnitsEvent : public Event
{
public:
	DeleteUnitsEvent();
	~DeleteUnitsEvent();

	//int getX() const { return num; };
	//int getY() const { return mY; };

};