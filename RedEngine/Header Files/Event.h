#pragma once

#include "Trackable.h"

enum EventType
{
    INVALID_EVENT_TYPE = -1,
    KEYBOARD_EVENT,
    MOUSE_EVENT,
    PLAYER_MOVE_EVENT,
    NUM_EVENT_TYPES
};

class Event : public Trackable
{

public:
    Event(EventType);
    virtual ~Event();

    EventType getType() const { return mType; }

private:
    Event() = delete;

    EventType mType;

};