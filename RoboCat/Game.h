#pragma once
#include <allegro5/allegro.h>

class Paddle;

class Game
{

	ALLEGRO_DISPLAY* mDisplay;
	Paddle* mPaddleOne;
	Paddle* mPaddleTwo;
	ALLEGRO_KEYBOARD_STATE* keyboardState;


	bool mRunning;
	bool mIsServer = false;
	bool mLocalLeft = true;

	bool InitAllegro();
	void ProcessLocalInput();
	void UpdateLocalPaddle();
	void Render();
	void CheckCollisions();
	void SendUpdatedStates();


public:
	Game();
	~Game();
	
	void Update();
	

};

