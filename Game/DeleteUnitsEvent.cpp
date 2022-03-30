#include "DeleteUnitsEvent.h"

DeleteUnitsEvent::DeleteUnitsEvent(int x, int y)
	:Event(DELETE_UNITS_EVENT)
	, mX(x)
	, mY(y)
{
}

DeleteUnitsEvent::~DeleteUnitsEvent()
{
}
