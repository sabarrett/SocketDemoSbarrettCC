#include "InputSystem.h"
#include "LibraryIncludes.h"
#include <iostream>
#include "EventSystem.h"
#include "KeyboardEvent.h"
#include "MouseEvent.h"

InputSystem* InputSystem::mspInstance = nullptr;

InputSystem::InputSystem()
{
	
}

InputSystem::~InputSystem()
{

}

InputSystem* InputSystem::getInstance()
{
	if(!mspInstance)
		mspInstance = new InputSystem;

	return mspInstance;
}

void InputSystem::cleanupInstance()
{
	if(mspInstance)
		delete mspInstance;
}

bool InputSystem::getKey(KeyCode key)
{
	return IsKeyDown(key);
}

bool InputSystem::getKeyDown(KeyCode key)
{
	return IsKeyPressed(key);
}

bool InputSystem::getKeyUp(KeyCode key)
{
	return IsKeyReleased(key);
}

bool InputSystem::getMouseButtonDown(int button)
{
	return IsMouseButtonPressed(button);
}

bool InputSystem::getMouseButtonUp(int button)
{
	return IsMouseButtonReleased(button);
}

bool InputSystem::getMouseButton(int button)
{
	return IsMouseButtonDown(button);
}

int InputSystem::getMouseX()
{
	return GetMouseX();
}

int InputSystem::getMouseY()
{
	return GetMouseY();
}

Vector2D InputSystem::getMousePosition()
{
	return Vector2D(GetMouseX(), getMouseY());
}

void InputSystem::inputUpdate()
{
	for(int i = Key_A; i != Key_Max; i++)
	{
		if(getKeyDown((KeyCode)i))
		{
			EventSystem::getInstance()->fireEvent(KeyboardEvent((KeyCode)i, BUTTON_DOWN));
		}
		else if(getKey((KeyCode)i))
		{
			EventSystem::getInstance()->fireEvent(KeyboardEvent((KeyCode)i, BUTTON_HELD));
		}
		else if(getKeyUp((KeyCode)i))
		{
			EventSystem::getInstance()->fireEvent(KeyboardEvent((KeyCode)i, BUTTON_UP));
		}
	}

	for(int i = 0; i <= MOUSE_BUTTON_MAX; i++)
	{
		if(getMouseButtonDown(i))
		{
			EventSystem::getInstance()->fireEvent(MouseEvent(i, BUTTON_DOWN));
		}
		else if(getMouseButton(i))
		{
			EventSystem::getInstance()->fireEvent(MouseEvent(i, BUTTON_HELD));
		}
		else if(getMouseButtonUp(i))
		{
			EventSystem::getInstance()->fireEvent(MouseEvent(i, BUTTON_UP));
		}
	}
}

void InputSystem::setHorizonalMovementAxis(float val)
{
	if(val > 1.0f)
		val = 1.0f;
	else if(val < -1.0f)
		val = -1.0f;

	mMovementAxis = Vector2D(val, mMovementAxis.getY());
}

void InputSystem::setVerticalMovementAxis(float val)
{
	if(val > 1.0f)
		val = 1.0f;
	else if(val < -1.0f)
		val = -1.0f;

	mMovementAxis = Vector2D(mMovementAxis.getX(), val);
}

void InputSystem::setMovementAxis(Vector2D axis)
{
	float x = axis.getX();
	float y = axis.getY();

	if(x > 1.0f)
		x = 1.0f;
	else if(x < -1.0f)
		x = -1.0f;

	if(y > 1.0f)
		y = 1.0f;
	else if(y < -1.0f)
		y = -1.0f;

	mMovementAxis = Vector2D(x, y);
}