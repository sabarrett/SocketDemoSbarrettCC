#pragma once

/*
Allegro Wrapper Functions
Written by Adel Talhouk in FA21
InputSystem.h

	File information:
	This file contains the keycodes for input, which can be used in any way desired by other classes
	and files.
*/

#include "GraphicsLibrary.h"

//Include allegro libraries for input
#include <allegro5/allegro.h>

#include "GameFiles/WorldState.h"
#include "GameFiles/JoinerInput.h"


enum KeyCode
{
	ESCAPE_KEY = ALLEGRO_KEY_ESCAPE,
	R = ALLEGRO_KEY_R
};

enum MouseButton
{
	None = -1,
	LeftMouse = 0,
	RightMouse = 1,
	MiddleMouse = 2
};

enum InputMode
{
	NONE = -1,
	KeyPressed = ALLEGRO_EVENT_KEY_DOWN,
	KeyReleased = ALLEGRO_EVENT_KEY_UP,
	MouseDown = ALLEGRO_EVENT_MOUSE_BUTTON_DOWN,
	MouseUp = ALLEGRO_EVENT_MOUSE_BUTTON_UP
};

class InputSystem
{
	//-------------------------Private data-------------------------

	//Event queue
	ALLEGRO_EVENT_QUEUE* mpEventQueue;

	//Event
	ALLEGRO_EVENT mEvent;

	//-------------------------Public data-------------------------
public:

	//Constructor(s)
	InputSystem();

	//Destructor
	~InputSystem();

	//Accessor(s)
	float getMouseX();
	float getMouseY();
	Location GetMousePosition();

	//Functions
	bool init(GraphicsLibrary* pGraphicsLib);
	MouseButton GetMouseInput();
	KeyCode getKeyboardInput();

	bool wasHoldingLeftMouseLastFrame = false;
	bool wasHoldingMiddleMouseLastFrame = false;
	bool wasHoldingRightMouseLastFrame = false;

	// custom function to handle the game's input - bb
	void Update(bool isCreator, WorldState& gameWorld, vector<JoinerInput>& joinerInputs);
};