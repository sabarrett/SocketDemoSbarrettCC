#pragma once
#include <allegro5/allegro.h>
#include "GraphicsSystems.h"

#include <vector>
#include <string>
#include <map>
#include <utility>

using std::vector;
using std::pair;
using std::string;
using std::map;

enum KEYCODES
{
	D = ALLEGRO_KEY_D,
	S = ALLEGRO_KEY_S,
	Esc = ALLEGRO_KEY_ESCAPE,
	UP_ARROW = ALLEGRO_KEY_UP,
	DOWN_ARROW = ALLEGRO_KEY_DOWN,
	LEFT_ARROW = ALLEGRO_KEY_LEFT,
	RIGHT_ARROW = ALLEGRO_KEY_RIGHT
};

class InputSystem
{
	ALLEGRO_EVENT_QUEUE* mpKeyboardEvents;
	
public:

	InputSystem();
	~InputSystem();

	bool InitInputSystem(GraphicsSystems* mpGraphicsSystem);

	void KeyUpdate();

	bool getKeyState(int keycode);

private:

	ALLEGRO_KEYBOARD_STATE* mpKeyboardState;

	int mMousePosY = 0, mMousePosX = 0;

	friend class GraphicSystems;
};
