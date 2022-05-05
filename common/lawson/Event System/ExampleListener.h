#pragma once

#include <Trackable.h>
#include "EventListener.h"

class EventSystem;

class ExampleListener : public EventListener
{
public:
	ExampleListener();
	~ExampleListener();

	void handleEvent( const Event& theEvent );

private:
};