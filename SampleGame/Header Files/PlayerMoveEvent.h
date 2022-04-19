#pragma once

#include "Event.h"
#include "Vector2D.h"

class PlayerMoveEvent : public Event
{

public:
    PlayerMoveEvent(Vector2D loc, float time);
    ~PlayerMoveEvent();

    Vector2D getMoveLoc() const { return moveLoc; }
    float getTime() const { return time; }

private:
    PlayerMoveEvent() = delete;

    Vector2D moveLoc;
    float time;

};