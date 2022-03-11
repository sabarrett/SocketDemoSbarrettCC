#pragma once
#include <iostream>

enum EventType
{
	INVALID_EVENT_TYPE = -1,
	MOVE_EVENT,
	MOUSE_PRESS_EVENT
};

class Event
{
public:
	Event();
	Event(EventType type) { mType = type; }
	virtual ~Event() {};

	EventType getType() const { return mType; }

private:
	EventType mType;
};

class MouseEvent
{
public:
	MouseEvent();
	MouseEvent(int buttonPressed) { mButtonPressed = buttonPressed; }
	~MouseEvent();

	int getMousePress() const { return mButtonPressed; }

private:
	int mButtonPressed;
};