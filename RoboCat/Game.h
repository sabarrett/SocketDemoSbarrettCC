#pragma once
#include "InputSystem.h"

const int SCREEN_SIZE_X = 1920;
const int SCREEN_SIZE_Y = 1080;

const std::string FILE_PATH = "..\\common\\assets\\";
const std::string BACKGROUND_FILE = "steps.png";
const std::string HOMER_FILE = "HomerSimpson.png";
const std::string QUIMBY_FILE = "Mayor_Quimby.png";
const std::string DONUT_FILE = "Donut.png";

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
	GraphicsLib* mpGraphicsLib2;
	InputSystem* mpInputSystem;

	bool isPlaying;
};