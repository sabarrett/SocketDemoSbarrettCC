#include "MoveEvent.h"

MoveEvent::MoveEvent(const Vector2D& loc)
:Event( MOVE_EVENT )
,mLoc(loc)
{
}

MoveEvent::~MoveEvent()
{
}
