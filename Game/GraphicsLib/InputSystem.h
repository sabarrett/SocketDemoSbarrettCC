#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <string>
#include "EventSystem.h"

#include "KeyEvent.h"
#include "MouseEvent.h"
#include "MouseMoveEvent.h"

using namespace std;

class InputSystem : public Trackable
{
public:
	InputSystem();
	~InputSystem();

	void init();
	void cleanup();

	void update();

private:
	ALLEGRO_KEYBOARD_STATE mKeyState;
	ALLEGRO_MOUSE_STATE mMouseState;

	EventSystem* mpEventSystem;
};