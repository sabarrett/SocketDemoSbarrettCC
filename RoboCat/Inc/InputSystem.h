#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	void init();
	void cleanup();
	void update();

	void updateMoseInput();
	bool getMouseState(int click);
	bool getMouseDown(int click);
	void getMouseUp(int click);

	int getMouseLocX() { return pmMouse->x; }
	int getMouseLocY() { return pmMouse->y; }

private:
	ALLEGRO_MOUSE_STATE* pmMouse = nullptr;
	ALLEGRO_MOUSE_STATE* pmStartMouse = nullptr;

	ALLEGRO_EVENT_QUEUE* mpQueue;
	bool isInit = false;
	bool rightMouseDown, leftMouseDown;
};