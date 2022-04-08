#pragma once

#include "Event.h"
#include "Vector2D.h"

class PlayerMoveEvent : public Event
{

public:
    PlayerMoveEvent(Vector2D loc);
    ~PlayerMoveEvent();

    Vector2D getMoveLoc() const { return moveLoc; }

private:
    PlayerMoveEvent() = delete;

    Vector2D moveLoc;

};