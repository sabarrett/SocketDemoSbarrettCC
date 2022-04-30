#include"InputTranslator.h"

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 5
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

InputTranslator::InputTranslator(InputSystem* input)
{
	//EventSystem::initInstance();
	mEventSystem = EventSystem::getInstance();
	mInputSystem = input;
	mDeleted = false;
}

InputTranslator::~InputTranslator()
{
	cleanup();
}

void InputTranslator::init()
{
	EventSystem::getInstance()->addListener(KEY_UP_EVENT, this);
	EventSystem::getInstance()->addListener(KEY_DOWN_EVENT, this);
	EventSystem::getInstance()->addListener(MOUSE_UP_EVENT, this);
	EventSystem::getInstance()->addListener(MOUSE_DOWN_EVENT, this);
	EventSystem::getInstance()->addListener(MOUSE_MOVE_EVENT, this);
}

void InputTranslator::cleanup()
{
	if (!mDeleted)
	{
		mEventSystem->removeListenerFromAllEvents(this);
		mDeleted = true;
	}
}

void InputTranslator::handleEvent(const Event& event)
{
	switch (event.getType())
	{
	case KEY_DOWN_EVENT:
	{
		const KeyEvent& myEvent = static_cast<const KeyEvent&>(event);

		if (myEvent.getKeyCode() == mInputSystem->KEY_SPACE)
		{
			GameEvent eventToFire(TOGGLE_ALL_UNIT_ANIMATIONS_EVENT);
			mEventSystem->fireEvent(eventToFire);
		}

		if (myEvent.getKeyCode() == mInputSystem->KEY_ENTER)
		{
			GameEvent eventToFire(CHANGE_UNIT_ANIMATION_EVENT);
			mEventSystem->fireEvent(eventToFire);
		}

		if (myEvent.getKeyCode() == mInputSystem->KEY_ESC)
		{
			GameEvent eventToFire(EXIT_GAME_EVENT);
			mEventSystem->fireEvent(eventToFire);
		}
	}
	break;

	case KEY_UP_EVENT:
	{
	}
	break;

	case MOUSE_DOWN_EVENT:
	{
		const ClickEvent& myEvent = static_cast<const ClickEvent&>(event);

		if (myEvent.getClick() == mInputSystem->MOUSE_1)
		{
			GameEvent eventToFire(CREATE_UNIT_EVENT);
			mEventSystem->fireEvent(eventToFire);
		}

		if (myEvent.getClick() == mInputSystem->MOUSE_2)
		{
			GameEvent eventToFire(DESTROY_UNIT_EVENT);
			mEventSystem->fireEvent(eventToFire);
		}
	}
	break;

	case MOUSE_UP_EVENT:
	{
	}
	break;

	case MOUSE_MOVE_EVENT:
	{
		const MouseEvent& myEvent = static_cast<const MouseEvent&>(event);
		GameEvent eventToFire(UPDATE_MOUSE_POSITION_EVENT, myEvent.getPosX(), myEvent.getPosY());
		mEventSystem->fireEvent(eventToFire);
	}
	break;

	default:
		GameEvent invalid(INVALID_TYPE_OF_EVENT);
		mEventSystem->fireEvent(invalid);
		break;
	}
}
