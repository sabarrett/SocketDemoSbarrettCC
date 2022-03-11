#pragma once
#include "EventSystem.h"

class Event;
//class EventSystem;

class EventListener
{
public:
	EventListener() {};
	virtual ~EventListener() { EventSystem::getInstance()->removeListenerFromAllEvents(this); }

	virtual void handleEvent(const Event& theEvent) = 0;
};