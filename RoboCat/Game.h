#pragma once
#include <allegro5/allegro.h>
#include "RoboCatPCH.h"


class Paddle;

class Game
{

	ALLEGRO_DISPLAY* mDisplay;
	Paddle* mPaddleOne;
	Paddle* mPaddleTwo;
	ALLEGRO_KEYBOARD_STATE* keyboardState;
	TCPSocketPtr TCPSocket;


	bool mRunning = false;
	bool mIsServer = false;
	bool mLocalLeft = true;

	bool InitAllegro();
	void StartServer();
	void ConnectToServer(std::string ip);
	void ProcessLocalInput();
	void UpdateLocalPaddle();
	void Render();
	void CheckCollisions();
	//void SendUpdatedStates();
	void DrawMenu();

	


public:
	Game();
	Game(std::string ip);
	~Game();
	void Update();
	

};

