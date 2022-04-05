#pragma once
#include "InputSystem.h"

class Game
{
public:
	Game();
	~Game();

	void init();
	void cleanup();
	void gameLoop();

	void input();
	void update();
	void render();
private:
	GraphicsLib* mpGraphicsLib;
	InputSystem* mpInputSystem;
};