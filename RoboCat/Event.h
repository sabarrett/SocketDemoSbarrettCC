#pragma once

#include <string>
#include <../../common/DeanLib/include/Trackable.h>

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 5
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

using namespace std;

enum EventType
{
	INVALID_EVENT_TYPE = -1,
	MOVE_EVENT,
	MESSAGE_EVENT,
	KEY_DOWN_EVENT,
	KEY_UP_EVENT,
	MOUSE_DOWN_EVENT,
	MOUSE_UP_EVENT,
	MOUSE_MOVE_EVENT,
	NUM_EVENT_TYPES
};

class Event:public Trackable
{
public:
	Event( EventType type );
	virtual ~Event();

	EventType getType() const { return mType; };

private:
	EventType mType;

};

class KeyEvent : public Event
{
public:
	KeyEvent(EventType type, int key);
	~KeyEvent();

	int getKeyCode() const { return mKeyCode; }
	bool getPressed() { return mIsPressed; }

	void setPressed(bool pressed) { mIsPressed = pressed; }
private:
	int mKeyCode;
	bool mIsPressed;
};

class ClickEvent : public Event
{
public:
	ClickEvent(EventType type, int click);
	~ClickEvent();

	int getClick() const { return mClick; }
	bool getisClick() const { return mIsClick; }

	void setIsClick(bool clicked) { mIsClick = clicked; }
private:
	bool mIsClick;
	int mClick;
};

class MouseEvent : public Event
{
public:
	MouseEvent(EventType type, int posX, int posY);
	~MouseEvent();

	int getPosX() const { return mPosX; }
	int getPosY() const { return mPosY; }
private:
	int mPosX;
	int mPosY;
};