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
	mpEventQueue = al_create_event_queue();
}

//Destructor
InputSystem::~InputSystem()
{
	//Cleanup event queue
	al_destroy_event_queue(mpEventQueue);
	mpEventQueue = nullptr;
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
	al_register_event_source(mpEventQueue, al_get_display_event_source(pGraphicsLib->mpDisplay));

	//Register keyboard event source
	al_register_event_source(mpEventQueue, al_get_keyboard_event_source());

	//Register mouse event source
	al_register_event_source(mpEventQueue, al_get_mouse_event_source());

	return true;
}

MouseButton InputSystem::getMouseInput()
{
	//If there is an event
	al_wait_for_event(mpEventQueue, &mEvent);

	if (mEvent.type == InputMode::MouseDown)
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

KeyCode InputSystem::getKeyboardInput()
{
	//If there is an event
	al_wait_for_event(mpEventQueue, &mEvent);
	
	if (mEvent.type == InputMode::KeyPressed)
	{
		//Check the type
		switch (mEvent.keyboard.keycode)
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

		default:
			/*return KeyCode::NONE*/;
		}
	}

	//return KeyCode::NONE;
}