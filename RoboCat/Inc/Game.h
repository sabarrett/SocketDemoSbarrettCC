//#pragma once
#include "SDL.h"
#undef main

#include <string>

class Game {

public:
	Game();
	~Game();

	void Init(char* title, int width, int height);
	void Update();
	void Draw();
	void Clean();

	
	void CreateGameWindow(char* title, int width, int height);
};