#include "KeyEvent.h"

KeyEvent::KeyEvent(const string key)
	:Event(1)
	, mKey(key)
{
}

KeyEvent::~KeyEvent()
{
}
