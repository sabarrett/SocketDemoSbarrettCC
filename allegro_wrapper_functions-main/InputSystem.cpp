/*
Allegro Wrapper Functions
Written by Adel Talhouk in FA21
InputSystem.cpp
*/
#include <iostream>

#include "RoboCatPCH.h"
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

//Update
void InputSystem::update()
{
	// Buttons that don't allow continuous press
	currentFrameData.keyPressed_ESCAPE = false;
	currentFrameData.keyPressed_R = false;
	currentFrameData.keyPressed_SPACE = false;

	switch (getKeyboardInput().keyCode)
	{
	case KeyCode::ESCAPE_KEY:
	{
		// Doesn't allow continuous press
		currentFrameData.keyPressed_ESCAPE = keyData.gotPressed;
		//std::cout << "Escape Pressed" << std::endl;
		break;
	}
	case KeyCode::A_KEY:
	{
		// Allows continuous press
		currentFrameData.keyPressed_A = keyData.gotPressed;
		//std::cout << "A Pressed" << std::endl;
		break;
	}
	case KeyCode::D_KEY:
	{
		// Allows continuous press
		currentFrameData.keyPressed_D = keyData.gotPressed;
		//std::cout << "D Pressed" << std::endl;
		break;
	}
	case KeyCode::R_KEY:
	{
		// Doesn't allow continuous press
		currentFrameData.keyPressed_R = keyData.gotPressed;
		//std::cout << "R Pressed" << std::endl;
		break;
	}
	case KeyCode::SPACE_KEY:
	{
		// Doesn't allow continuous press
		currentFrameData.keyPressed_SPACE = keyData.gotPressed;
		//std::cout << "Space Pressed" << std::endl;
		break;
	}
	default:
		break;
	}
}

InputData InputSystem::getInputData()
{
	return currentFrameData;
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

KeyData InputSystem::getKeyboardInput()
{
	//If there is an event
	al_wait_for_event_timed(mpEventQueue, &mEvent, 0.000001);

	if (mEvent.type == InputMode::KeyPressed)
	{
		keyData.gotPressed = true;

		//Check the type
		switch (mEvent.keyboard.keycode)
		{
		case KeyCode::ESCAPE_KEY:
			keyData.keyCode = KeyCode::ESCAPE_KEY;
			break;

		case KeyCode::R_KEY:
			keyData.keyCode = KeyCode::R_KEY;
			break;

		case KeyCode::A_KEY:
			keyData.keyCode = KeyCode::A_KEY;
			break;

		case KeyCode::D_KEY:
			keyData.keyCode = KeyCode::D_KEY;
			break;

		case KeyCode::SPACE_KEY:
			keyData.keyCode = KeyCode::SPACE_KEY;
			break;
		default:
			/*return KeyCode::NONE*/;
		}
	}
	else if (mEvent.type == InputMode::KeyReleased)
	{
		keyData.gotPressed = false;

		//Check the type
		switch (mEvent.keyboard.keycode)
		{
		case KeyCode::ESCAPE_KEY:
			keyData.keyCode = KeyCode::ESCAPE_KEY;
			break;

		case KeyCode::R_KEY:
			keyData.keyCode = KeyCode::R_KEY;
			break;

		case KeyCode::A_KEY:
			keyData.keyCode = KeyCode::A_KEY;
			break;

		case KeyCode::D_KEY:
			keyData.keyCode = KeyCode::D_KEY;
			break;

		case KeyCode::SPACE_KEY:
			keyData.keyCode = KeyCode::SPACE_KEY;
			break;
		default:
			/*return KeyCode::NONE*/;
		}
	}

	return keyData;
	//return KeyCode::NONE;
}