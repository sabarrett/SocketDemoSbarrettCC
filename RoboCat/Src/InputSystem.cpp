/*
Allegro Wrapper Functions
Written by Adel Talhouk in FA21
InputSystem.cpp
*/

#include "InputSystem.h"
#include "RoboCatPCH.h"

#include <iostream>

//Constructor
InputSystem::InputSystem()
{
	//Create an event queue
	mpMouseEventQueue = al_create_event_queue();
	mpKeyboardEventQueue = al_create_event_queue();
}

//Destructor
InputSystem::~InputSystem()
{
	//Cleanup event queues
	al_destroy_event_queue(mpMouseEventQueue);
	mpMouseEventQueue = nullptr;
	al_destroy_event_queue(mpKeyboardEventQueue);
	mpKeyboardEventQueue = nullptr;
}

float InputSystem::getMouseX()
{ 
	//Update mouse state
	ALLEGRO_MOUSE_STATE mouseState;
	al_get_mouse_state(&mouseState);

	return mouseState.x;
}

float InputSystem::getMouseY() 
{
	//Update mouse state
	ALLEGRO_MOUSE_STATE mouseState;
	al_get_mouse_state(&mouseState);

	return mouseState.y;
}

std::pair<float, float> InputSystem::getMousePosition()
{
	//Update mouse state
	ALLEGRO_MOUSE_STATE mouseState;
	al_get_mouse_state(&mouseState);

	return std::make_pair(mouseState.x, mouseState.y);
}

//Init
bool InputSystem::init(GraphicsLibrary* pGraphicsLib)
{
	//Init keyboard
	if (!al_install_keyboard())
	{
		std::cout << "error installing Allegro keyboard plugin\n";
		system("pause");
		return false;
	}

	//Init mouse
	if (!al_install_mouse())
	{
		std::cout << "error installing Allegro mouse plugin\n";
		system("pause");
		return false;
	}

	//Register screen event source
	al_register_event_source(mpMouseEventQueue, al_get_display_event_source(pGraphicsLib->mpDisplay));
	al_register_event_source(mpKeyboardEventQueue, al_get_display_event_source(pGraphicsLib->mpDisplay));

	//Register mouse event source
	al_register_event_source(mpMouseEventQueue, al_get_mouse_event_source());

	//Register keyboard event source
	al_register_event_source(mpKeyboardEventQueue, al_get_keyboard_event_source());

	return true;
}

MouseButton InputSystem::getMouseInput()
{
	//If there is an event
	al_get_next_event(mpMouseEventQueue, &mMouseEvent);

	if (mMouseEvent.type == InputMode::MouseDown)
	{
		//Update mouse state
		ALLEGRO_MOUSE_STATE mouseState;
		al_get_mouse_state(&mouseState);

		//Check the button pressed
		if (mouseState.buttons & 1)		//Left mouse held
		{
			return MouseButton::LeftMouse;
		}
		else if (mouseState.buttons & 2)	//Right mouse held
		{
			return MouseButton::RightMouse;
		}
		else if (mouseState.buttons & 4)	//Middle mouse held
		{
			return MouseButton::MiddleMouse;
		}
	}
}

KeyCode InputSystem::getKeyboardInput(InputMode inputMode)
{
	//If there is an event
	al_get_next_event(mpKeyboardEventQueue, &mKeyboardEvent);
	
	if (mKeyboardEvent.type == inputMode)
	{
		//Check the type
		switch (mKeyboardEvent.keyboard.keycode)
		{
		case KeyCode::Esc:
			return KeyCode::Esc;
			break;

		case KeyCode::R:
			return KeyCode::R;
			break;

		case KeyCode::Tab:
			return KeyCode::Tab;
			break;

		case KeyCode::W:
			return KeyCode::W;
			break;

		case KeyCode::A:
			return KeyCode::A;
			break;

		case KeyCode::S:
			return KeyCode::S;
			break;

		case KeyCode::D:
			return KeyCode::D;
			break;

		default:
		}
	}

}