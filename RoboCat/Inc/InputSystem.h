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
};

class InputSystem
{
	ALLEGRO_EVENT_QUEUE* mpKeyboardEvents;
	
public:

	InputSystem();
	~InputSystem();

	bool initInputSystem(GraphicsSystems* mpGraphicsSystem);
	void cleanup();

	void keyUpdate();

	bool getKeyState(int keycode);

private:

	ALLEGRO_KEYBOARD_STATE* mpKeyboardState;

	int mMousePosY = 0, mMousePosX = 0;

	friend class GraphicSystems;
};
