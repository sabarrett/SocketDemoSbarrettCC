#include "PlaceUnitEvent.h"

PlaceUnitEvent::PlaceUnitEvent(int x, int y)
	:Event(PLACE_UNIT_EVENT)
	, mX(x)
	, mY(y)
{
}

PlaceUnitEvent::~PlaceUnitEvent()
{
}
