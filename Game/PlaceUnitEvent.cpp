#include "PlaceUnitEvent.h"

PlaceUnitEvent::PlaceUnitEvent(int num)
	:Event(6)
	, type(num)
{
}

PlaceUnitEvent::~PlaceUnitEvent()
{
}
