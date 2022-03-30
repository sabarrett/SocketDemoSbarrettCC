#include "MouseEvent.h"

MouseEvent::MouseEvent(const int mouseButton, int x, int y)
	:Event(MOUSE_EVENT_B)
	, mMouseButton(mouseButton)
	, mX(x)
	, mY(y)
{
}

MouseEvent::~MouseEvent()
{
}
