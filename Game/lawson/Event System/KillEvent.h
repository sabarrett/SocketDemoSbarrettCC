#pragma once

#include "Event.h"
#include "GameEvent.h"

class KillEvent : public Event
{
public:
	KillEvent() :Event((EventType)KILL) {};
	~KillEvent() {};
private:
};