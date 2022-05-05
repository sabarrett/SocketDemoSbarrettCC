#include "RoboCatPCH.h"

/*
Allegro Wrapper Functions
Written by Adel Talhouk in FA21
InputSystem.cpp
*/
#include "InputSystem.h"
#include "GameController.h"

#include <iostream>

//Constructor
InputSystem::InputSystem()
{
	//Create an event queue
	mpKeyboardEventQueue = al_create_event_queue();
}

//Destructor
InputSystem::~InputSystem()
{
	//Cleanup event queue
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
	al_register_event_source(mpKeyboardEventQueue, al_get_display_event_source(pGraphicsLib->mpDisplay));

	//Register keyboard event source
	al_register_event_source(mpKeyboardEventQueue, al_get_keyboard_event_source());

	//Register mouse event source
	al_register_event_source(mpKeyboardEventQueue, al_get_mouse_event_source());

	//mGameController = GameController();

	return true;
}

MouseButton InputSystem::getMouseInput()
{
	//If there is an event
	al_wait_for_event(mpKeyboardEventQueue, &mKeyboardEvent);

	if (mKeyboardEvent.type == InputMode::MouseDown)
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
	//al_wait_for_event(mpEventQueue, &mEvent);
	al_get_next_event(mpKeyboardEventQueue, &mKeyboardEvent);
	if (mKeyboardEvent.type == inputMode)
	{
		return (KeyCode)mKeyboardEvent.keyboard.keycode;
	}

	//return pressKey;
}