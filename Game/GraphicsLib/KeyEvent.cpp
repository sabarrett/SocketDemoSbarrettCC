#include "KeyEvent.h"

KeyEvent::KeyEvent(const string key)
	:Event(KEY_EVENT_B)
	, mKey(key)
{
}

KeyEvent::~KeyEvent()
{
}
