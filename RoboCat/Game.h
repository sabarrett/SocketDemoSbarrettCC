#pragma once
#include <allegro5/allegro.h>
#include "RoboCatPCH.h"


class Paddle;
class ball;

class Game
{

	ALLEGRO_DISPLAY* mDisplay;
	Paddle* mPaddleOne;
	Paddle* mPaddleTwo;
	ball* mBall;

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
	void UpdateBall();
	void Render();
	void CheckCollisions(ball* ball, Paddle* paddle);
	void SendUpdatedStates();
	void Receive(); 

	


public:
	Game();
	Game(std::string ip);
	~Game();
	void Update();
	void UpdateNetworkedPaddle(int y);
	void UpdateNetworkedBall(int x, int y);
	

};

