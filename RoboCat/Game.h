#pragma once
#include <allegro5/allegro.h>
#include "RoboCatPCH.h"


class Paddle;
class ball;
class Score;

class Game
{

	ALLEGRO_DISPLAY* mDisplay;
	Paddle* mPaddleOne;
	Paddle* mPaddleTwo;
	Score* mScoreOne;
	Score* mScoreTwo;
	ball** mBalls;

	ALLEGRO_KEYBOARD_STATE* keyboardState;
	TCPSocketPtr TCPSocket;


	bool mRunning = false;
	bool mIsServer = false;
	bool mLocalLeft = true;
	int mWinningPlayer = 0;

	bool InitAllegro();
	void StartServer();
	void ConnectToServer(std::string ip);
	void ProcessLocalInput();
	void UpdateLocalPaddle();
	void UpdateBall(ball* ball);
	void UpdateScore();
	void Render();
	void CheckCollisions(ball* ball);
	void CheckCollisionsPaddle(ball* ball, Paddle* paddle1, Paddle* paddle2);
	void SendUpdatedStates();
	int Send(const void *inData, size_t inLen);
	void Receive(); 

	


public:
	Game();
	Game(std::string ip);
	~Game();
	void Update();
	void UpdateNetworkedPaddle(int y);
	

};

