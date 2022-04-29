#include "StringEvent.h"

StringEvent::StringEvent(const string& theString)
	:Event( MESSAGE_EVENT )
,mString(theString)
{
}

StringEvent::~StringEvent()
{
}
