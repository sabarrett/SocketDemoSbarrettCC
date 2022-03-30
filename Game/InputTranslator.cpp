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
	//std::cout << "handeling an event :)" << theEvent.getType() << " " << MOUSE_EVENT_B << endl;
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
	else if (theEvent.getType() == 1)
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
		else if (keyEvent.getState() == "1")
		{
			std::cout << "handeling an event for button	1" << endl;
			//fire exit game event
			mpESystem->fireEvent(EscEvent());
		}
		else if (keyEvent.getState() == "2")
		{
			std::cout << "handeling an event for button	2" << endl;
			//fire exit game event
			mpESystem->fireEvent(EscEvent());
		}
		else if (keyEvent.getState() == "3")
		{
			std::cout << "handeling an event for button	3" << endl;
			//fire exit game event
			mpESystem->fireEvent(EscEvent());
		}
	}
}
