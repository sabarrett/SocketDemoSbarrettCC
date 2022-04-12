#include "WASDEvent.h"

WASDEvent::WASDEvent(Vector2D newDir)
	:Event(20)
	, dir(newDir)
{
}

WASDEvent::~WASDEvent()
{
}
