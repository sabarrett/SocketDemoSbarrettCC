#include "PlayerMoveEvent.h"

PlayerMoveEvent::PlayerMoveEvent(Vector2D loc, float time)
    : Event(PLAYER_MOVE_EVENT)
{
    moveLoc = loc;
    this->time = time;
}

PlayerMoveEvent::~PlayerMoveEvent()
{
    
}