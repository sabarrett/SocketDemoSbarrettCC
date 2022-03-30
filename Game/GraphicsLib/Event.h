#pragma once

#include <string>
#include <iostream>

using namespace std;

enum EventType
{
	//Input events
	MOUSE_EVENT_B,
	MOUSE_MOVE_EVENT_B,
	KEY_EVENT_B,

	//Game events
	PLACE_UNIT_EVENT,
	DELETE_UNITS_EVENT,
	SWITCH_ANIM_EVENT,
	ESC_EVENT,
	PAUSE_EVENT,
	TIME_CHANGE_EVENT,

	NUM_EVENT_TYPES_B
};

class Event 
{
public:
	Event(int type);
	virtual ~Event();

	int getType() const { std::cout << "this is a test " << mType << endl; return mType; };

private:
	int mType;

};