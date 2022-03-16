#include"InputSystem.h"
#include"Event.h"
#include"EventSystem.h"

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 5
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

InputSystem::InputSystem()
{
	mpEventQueue = nullptr;

	mPosX = 0;
	mPosY = 0;

	mDeleted = false;
}

InputSystem::~InputSystem()
{
	cleanup();
}

void InputSystem::init()
{
	mpEventQueue = al_create_event_queue();
	al_register_event_source(mpEventQueue, al_get_keyboard_event_source());
	al_register_event_source(mpEventQueue, al_get_mouse_event_source());
}

void InputSystem::cleanup()
{
	if (!mDeleted)
	{
		al_uninstall_keyboard();
		al_uninstall_mouse();

		al_unregister_event_source(mpEventQueue, al_get_keyboard_event_source());
		al_unregister_event_source(mpEventQueue, al_get_mouse_event_source());
		al_destroy_event_queue(mpEventQueue);

		mDeleted = true;
	}
}

void InputSystem::update()
{
	ALLEGRO_EVENT event;

	while (al_get_next_event(mpEventQueue, &event))
	{
		switch (event.type)
		{
		case ALLEGRO_EVENT_KEY_UP:
		{
			KeyEvent myEvent(KEY_UP_EVENT, event.keyboard.keycode);
			myEvent.setPressed(false);
		}
		break;

		case ALLEGRO_EVENT_KEY_DOWN:
		{
			KeyEvent myEvent(KEY_DOWN_EVENT, event.keyboard.keycode);
			EventSystem::getInstance()->fireEvent(myEvent);
		}
		break;

		case ALLEGRO_EVENT_MOUSE_AXES:
		{
			MouseEvent mouseEvent(MOUSE_MOVE_EVENT, event.mouse.x, event.mouse.y);
			EventSystem::getInstance()->fireEvent(mouseEvent);
		}
		break;

		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
		{
			ClickEvent myEvent(MOUSE_UP_EVENT, event.mouse.button);
			EventSystem::getInstance()->fireEvent(myEvent);
		}
		break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		{
			ClickEvent myEvent(MOUSE_DOWN_EVENT, event.mouse.button);
			EventSystem::getInstance()->fireEvent(myEvent);
		}
		break;

		}
	}
}