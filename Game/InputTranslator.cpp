#include "InputTranslator.h"
#include "Event.h"
#include "MouseEvent.h"
#include "KeyEvent.h"


InputTranslator::InputTranslator()
{
	mpESystem = EventSystem::getInstance();
	mpESystem->addListener(MOUSE_EVENT_B, this);
	mpESystem->addListener(KEY_EVENT_B, this);
}

InputTranslator::~InputTranslator()
{
}

void InputTranslator::init()
{
}

void InputTranslator::cleanup()
{
	EventSystem::getInstance()->removeListenerFromAllEvents(this);
}

void InputTranslator::handleEvent(const Event& theEvent)
{
	//Check for mouse Event
	if (theEvent.getType() == MOUSE_EVENT_B)
	{
		const MouseEvent& mouseEvent = static_cast<const MouseEvent&>(theEvent);
		if (mouseEvent.getState() == 1)
		{
			//fire place unit game event
			mpESystem->fireEvent(PlaceUnitEvent(mouseEvent.getX(),mouseEvent.getY()));
		}
		else if (mouseEvent.getState() == 2)
		{
			//fire delete unit game event 
			mpESystem->fireEvent(DeleteUnitsEvent(mouseEvent.getX(), mouseEvent.getY()));
		}
	}
	//Check for keyboard event
	else if (theEvent.getType() == KEY_EVENT_B)
	{
		const KeyEvent& keyEvent = static_cast<const KeyEvent&>(theEvent);
		if (keyEvent.getState() == "space")
		{
			//fire pause game event
			mpESystem->fireEvent(PauseEvent());
		}
		else if (keyEvent.getState() == "enter")
		{
			//fire switch animation event
			mpESystem->fireEvent(SwitchAnimEvent());
		}
		else if (keyEvent.getState() == "esc")
		{
			//fire exit game event
			mpESystem->fireEvent(EscEvent());
		}
	}
}
