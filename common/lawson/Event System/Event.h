#pragma once

#include <string>
#include <trackable.h>

using namespace std;

enum EventType
{
	INVALID_EVENT_TYPE = -1,
	MOVE_EVENT,
	MESSAGE_EVENT,
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