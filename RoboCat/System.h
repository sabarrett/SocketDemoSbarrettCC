#pragma once
#include "GraphicsSystem.h"
#include "InputSystem.h"
#include "../common/DeanLib/include/Trackable.h"
#include "../common/DeanLib/include/DeanLibDefines.h"
#include "../common/DeanLib/include/DeanLibUtilities.h"
#include "../common/DeanLib/include/DeanMath.h"

/*
Author: Wesley Elmer
	Class : Game Architecture <250-71>
	Assignment : Assignment 3
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

// The class which holds the Graphics System and has functions to get keyboard/mouse state �part of GraphicsLib project.
class System : public Trackable
{
public:
	System();
	~System();
	void init();
	void cleanup();

	GraphicsSystem* getGraphicsSystem() { return mGraphicsSystem; }
	InputSystem* getInputSystem() { return mInput; }

private:
	GraphicsSystem* mGraphicsSystem;
	InputSystem* mInput;

	ALLEGRO_MOUSE_STATE mMouseState;
	ALLEGRO_KEYBOARD_STATE mKeyboardState;
	bool mIsDeleted;
};

