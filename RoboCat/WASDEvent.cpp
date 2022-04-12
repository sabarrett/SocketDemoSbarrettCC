#include "WASDEvent.h"

WASDEvent::WASDEvent(Vector2D newDir)
	:Event(6)
	, dir(newDir)
{
}

WASDEvent::~WASDEvent()
{
}
