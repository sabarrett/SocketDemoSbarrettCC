/*
Allegro Wrapper Functions
Written by Adel Talhouk in FA21
InputSystem.cpp
*/
#include "RoboCatPCH.h"

#include <iostream>

#include "InputSystem.h"

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

Location InputSystem::GetMousePosition()
{
	//Update mouse state
	ALLEGRO_MOUSE_STATE mouseState;
	al_get_mouse_state(&mouseState);
	Location mouseLocation{ mouseState.x, mouseState.y };
	return mouseLocation;
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

MouseButton InputSystem::GetMouseInput()
{
	ALLEGRO_MOUSE_STATE state;

	al_get_mouse_state(&state);
	if (state.buttons & 1) 
	{
		return MouseButton::LeftMouse;
	}
	if (state.buttons & 4)
	{
		return MouseButton::MiddleMouse;
	}
	if (state.buttons & 2) 
	{
		return MouseButton::RightMouse;
	}
	
	return MouseButton::None;
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
		case KeyCode::ESCAPE_KEY:
			return KeyCode::ESCAPE_KEY;
			break;

		case KeyCode::R:
			return KeyCode::R;
			break;

		default:
			/*return KeyCode::NONE*/;
		}
	}

	//return KeyCode::NONE;
}

void InputSystem::Update(bool isCreator, WorldState& gameWorld, vector<JoinerInput>& joinerInputs)
{
	switch (GetMouseInput())
	{
	case MouseButton::LeftMouse:
		if (!wasHoldingLeftMouseLastFrame)
		{
			wasHoldingLeftMouseLastFrame = true;
			std::cout << "\nLeft Clicked\n";

			if (isCreator)
			{
				// create lock
				gameWorld.CreateLock();

			}
			else
			{
				// create key
				joinerInputs.push_back(JoinerInput(JoinerInput::JOINER_KEY_SPAWN, GetMousePosition()));
			}
		}
		break;
	case MouseButton::MiddleMouse:
		if (!wasHoldingMiddleMouseLastFrame)
		{
			wasHoldingMiddleMouseLastFrame = true;
			std::cout << "\nMiddle Clicked\n";
		}
		break;
	case MouseButton::RightMouse:
		if (!wasHoldingRightMouseLastFrame)
		{
			wasHoldingRightMouseLastFrame = true;
			std::cout << "\nRight Clicked\n";
		}
		break;
	case MouseButton::None:
		wasHoldingLeftMouseLastFrame = false;
		wasHoldingMiddleMouseLastFrame = false;
		wasHoldingRightMouseLastFrame = false;
		break;
	}
}