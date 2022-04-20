#include "MouseMoveEvent.h"

MouseMoveEvent::MouseMoveEvent(const Vector2D& loc)
	:Event(MOUSE_MOVE_EVENT_B)
	, mLoc(loc)
{
}

MouseMoveEvent::~MouseMoveEvent()
{
}
