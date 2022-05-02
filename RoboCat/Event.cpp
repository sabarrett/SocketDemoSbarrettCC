#include "Event.h"

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 5
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

Event::Event(EventType type)
:mType(type)
{
}

Event::~Event()
{
}

KeyEvent::KeyEvent(EventType type, int key) : Event(type)
{
	mKeyCode = key;
	mIsPressed = true;
}

KeyEvent::~KeyEvent()
{
}

ClickEvent::ClickEvent(EventType type, int click) : Event(type)
{
	mClick = click;
	mIsClick = true;
}

ClickEvent::~ClickEvent()
{
}

MouseEvent::MouseEvent(EventType type, int posX, int posY) : Event(type)
{
	mPosX = posX;
	mPosY = posY;
}

MouseEvent::~MouseEvent()
{
}