#pragma once
#include <allegro5/allegro.h>
#include <string>

class Paddle;

class Game
{

	ALLEGRO_DISPLAY* mDisplay;
	Paddle* mPaddleOne;
	Paddle* mPaddleTwo;
	ALLEGRO_KEYBOARD_STATE* keyboardState;


	bool mRunning = false;
	bool mIsServer = false;
	bool mLocalLeft = true;

	bool InitAllegro();
	void ProcessLocalInput();
	void UpdateLocalPaddle();
	void Render();
	void CheckCollisions();
	//void SendUpdatedStates();
	void DrawMenu();

	//void ProcessMenuInput();


public:
	Game();
	Game(std::string ip);
	~Game();
	void Update();
	

};

