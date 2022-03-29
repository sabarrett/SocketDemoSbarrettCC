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
	currentFrameData.clean();

	switch (getKeyboardInput())
	{
	case KeyCode::ESCAPE_KEY:
	{
		// Doesn't allow continuous press
		if (!lastFrameData.keyPressed_ESCAPE)
			currentFrameData.keyPressed_ESCAPE = true;
		//std::cout << "Escape Pressed" << std::endl;
		break;
	}
	case KeyCode::A_KEY:
	{
		// Allows continuous press
		currentFrameData.keyPressed_A = true;
		//std::cout << "A Pressed" << std::endl;
		break;
	}
	case KeyCode::D_KEY:
	{
		// Allows continuous press
		currentFrameData.keyPressed_D = true;
		//std::cout << "D Pressed" << std::endl;
		break;
	}
	case KeyCode::R_KEY:
	{
		// Doesn't allow continuous press
		if (!lastFrameData.keyPressed_R)
			currentFrameData.keyPressed_R = true;
		//std::cout << "R Pressed" << std::endl;
		break;
	}
	case KeyCode::SPACE_KEY:
	{
		// Doesn't allow continuous press
		if(!lastFrameData.keyPressed_SPACE)
			currentFrameData.keyPressed_SPACE = true;
		//std::cout << "Space Pressed" << std::endl;
		break;
	}
	default:
		break;
	}
}

InputData InputSystem::getInputData()
{
	lastFrameData = currentFrameData;
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

KeyCode InputSystem::getKeyboardInput()
{
	//If there is an event
	al_wait_for_event_timed(mpEventQueue, &mEvent, 0.000001);

	if (mEvent.type == InputMode::KeyPressed)
	{
		//Check the type
		switch (mEvent.keyboard.keycode)
		{
		case KeyCode::ESCAPE_KEY:
			return KeyCode::ESCAPE_KEY;
			break;

		case KeyCode::R_KEY:
			return KeyCode::R_KEY;
			break;

		case KeyCode::A_KEY:
			return KeyCode::A_KEY;
			break;

		case KeyCode::D_KEY:
			return KeyCode::D_KEY;
			break;

		case KeyCode::SPACE_KEY:
			return KeyCode::SPACE_KEY;
			break;
		default:
			/*return KeyCode::NONE*/;
		}
	}

	//return KeyCode::NONE;
}