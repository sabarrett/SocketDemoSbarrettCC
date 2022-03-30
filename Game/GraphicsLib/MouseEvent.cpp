#include "MouseEvent.h"

MouseEvent::MouseEvent(const int mouseButton, int x, int y)
	:Event(0)
	, mMouseButton(mouseButton)
	, mX(x)
	, mY(y)
{
}

MouseEvent::~MouseEvent()
{
}
