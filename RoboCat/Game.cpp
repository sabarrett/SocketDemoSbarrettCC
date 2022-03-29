#include "Game.h"
#include "Paddle.h"
#include "Ball.h"
//#include "MemoryBitStream.h"
#include <iostream>
#include <cmath>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <thread>
#include "Score.h"

enum class PacketType
{
	PT_PADDLE,
	PT_BALL,
	PT_SCORE
};

Game::Game()
{
	

	if (InitAllegro())
	{
		ALLEGRO_COLOR paddleColor = al_map_rgb(255, 255, 255);
		mPaddleOne = new Paddle(20, 100, paddleColor);
		mPaddleOne->SetPosition(30, 100);

		mPaddleTwo = new Paddle(20, 100, paddleColor);
		mPaddleTwo->SetPosition(al_get_display_width(mDisplay) - 50, 100);

		mBalls = new ball*[10];
		srand(static_cast <unsigned> (time(0)));
		float random = -0.5 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (0.5 - (-0.5))));
		for (int i = 0; i < 10; i++) {
			mBalls[i] = new ball(-0.05f + random, -0.05f, 15 + 1, paddleColor);
			mBalls[i]->id = i;
			mBalls[i]->pos = new position();
			mBalls[i]->pos->y = al_get_display_height(mDisplay) / 2 + i * 25;
			mBalls[i]->pos->x = al_get_display_width(mDisplay) / 2;
		}

		//mScoreOne = new Score(al_get_display_width(mDisplay) / 2 - 50, al_get_display_height(mDisplay) - 100, paddleColor);
		//mScoreTwo = new Score(al_get_display_width(mDisplay) / 2 + 50, al_get_display_height(mDisplay) - 100, paddleColor);
		//mScoreOne->str = "1";
		//mScoreTwo->str = "2";

		keyboardState = new ALLEGRO_KEYBOARD_STATE();
		mRunning = true;
	}


	mIsServer = true;
	mLocalLeft = mIsServer;
	StartServer();

}

Game::Game(std::string IP)
{
	if (InitAllegro())
	{
		ALLEGRO_COLOR paddleColor = al_map_rgb(255, 255, 255);
		mPaddleOne = new Paddle(20, 100, paddleColor);
		mPaddleOne->SetPosition(30, 100);

		mPaddleTwo = new Paddle(20, 100, paddleColor);
		mPaddleTwo->SetPosition(al_get_display_width(mDisplay) - 50, 100);

		keyboardState = new ALLEGRO_KEYBOARD_STATE();

		mBalls = new ball*[10];
		for (int i = 0; i < 10; i++) {
			mBalls[i] = new ball(0.0f, 0.0f, 15, paddleColor);
			mBalls[i]->id = i;
			mBalls[i]->pos = new position();
			mBalls[i]->pos->y = al_get_display_height(mDisplay) / 2 + i * 15;
			mBalls[i]->pos->x = al_get_display_width(mDisplay) / 2;
		}

		//mScoreOne = new Score(al_get_display_width(mDisplay) / 2 - 50, al_get_display_height(mDisplay) - 100, paddleColor);
		//mScoreTwo = new Score(al_get_display_width(mDisplay) / 2 + 50, al_get_display_height(mDisplay) - 100, paddleColor);

		mRunning = true;
	}
	mIsServer = false;
	mLocalLeft = mIsServer;
	ConnectToServer(IP);
}

Game::~Game()
{
	al_destroy_display(mDisplay);
	delete mPaddleOne, mPaddleTwo;
}

void Game::ProcessLocalInput()
{
	al_get_keyboard_state(keyboardState);

	if (al_key_down(keyboardState, ALLEGRO_KEY_SPACE))
	{
		mRunning = false;
	}

	if (al_key_down(keyboardState, ALLEGRO_KEY_TAB))
	{
		mLocalLeft = !mLocalLeft;
	}
	
	UpdateLocalPaddle();

}

void Game::UpdateLocalPaddle()
{
	int moveState = 0;

	if (al_key_down(keyboardState, ALLEGRO_KEY_UP))
	{
		moveState += 1;

		if (moveState > 1)
			moveState = 1;
	}

	if (al_key_down(keyboardState, ALLEGRO_KEY_DOWN))
	{
		moveState -= 1;

		if (moveState < -1)
			moveState = -1;
	}
	
	Paddle* localPaddle;
	
	if (mLocalLeft)
	{
		localPaddle = mPaddleOne;
	}
	else
	{
		localPaddle = mPaddleTwo;
	}

	
	int height = localPaddle->GetPosY();

	if (moveState == 1)
	{
		if (height > 0)
		{
			localPaddle->SetPosition(localPaddle->GetPosX(), height - 1);
		}
	}

	if (moveState == -1)
	{
		if (height + localPaddle->GetDimsW() < al_get_display_height(mDisplay))
		{
			localPaddle->SetPosition(localPaddle->GetPosX(), height + 1);
		}
	}
}

void Game::UpdateBall(ball* ball)
{
	ball->pos->x = ball->GetPosX() + ball->GetDirX();
	ball->pos->y = ball->GetPosY() + ball->GetDirY();

	OutputMemoryBitStream oStream;
	oStream.Write(PacketType::PT_BALL);
	oStream.Write(ball->id);
	oStream.Write(ball->pos->x);
	oStream.Write(ball->pos->y);

	TCPSocket->Send(oStream.GetBufferPtr(), oStream.GetBitLength());
}

//This math is for sure off, but this isn't a game physics class so I don't want to spend too much time
void Game::CheckCollisionsPaddle(ball* ball, Paddle* paddle1, Paddle* paddle2)
{
	if (ball->GetPosX() + ball->GetRadius() <= paddle1->GetPosX()) 
	{
		if(ball->GetPosY() >= (float)(paddle1->GetPosY() - paddle1->GetDimsL() / 2) && ball->GetPosY() <= (float)(paddle1->GetPosY() + paddle1->GetDimsL() / 2))
			ball->SetMovementVector(ball->GetDirX() * -1, ball->GetDirY());
	}
	if (ball->GetPosX() - ball->GetRadius() >= paddle2->GetPosX())
	{
		if (ball->GetPosY() >= paddle2->GetPosY() - paddle2->GetDimsL() / 2 && ball->GetPosY() <= paddle2->GetPosY() + paddle2->GetDimsL() / 2)
			ball->SetMovementVector(ball->GetDirX() * -1, ball->GetDirY());
	}
}

void Game::SendUpdatedStates()
{

	Paddle* localPaddle;

	if (mLocalLeft)
	{
		localPaddle = mPaddleOne;
	}
	else
	{
		localPaddle = mPaddleTwo;
	}


	int yPos = localPaddle->GetPosY();

	OutputMemoryBitStream oStream;
	oStream.Write(PacketType::PT_PADDLE);
	oStream.Write(yPos);

	TCPSocket->Send(oStream.GetBufferPtr(), oStream.GetBitLength());
}

void Game::Receive()
{
	while (mRunning)
	{
		char buffer[1024];
		int32_t bytesReceived = TCPSocket->Receive(buffer, 1024);
		if (bytesReceived > 0)
		{
			InputMemoryBitStream iStream = InputMemoryBitStream(buffer, 1024);
			PacketType type;
			iStream.Read(type);


			if (type == PacketType::PT_PADDLE)
			{
				int yPos;
				iStream.Read(yPos);

				UpdateNetworkedPaddle(yPos);
			}
			if (type == PacketType::PT_BALL) 
			{
				int tempID;
				iStream.Read(tempID);
				iStream.Read(mBalls[tempID]->pos->x);
				iStream.Read(mBalls[tempID]->pos->y);
			}
		}
		else  if (bytesReceived < 0)
		{
			if (bytesReceived == -10035)
			{

			}
			else if (bytesReceived == -10054)
			{
				mRunning = false;
				LOG("User disconnected: %s", "<Insert ID here>");
			}
		}
	}
	
}

void Game::Render()
{
	ALLEGRO_COLOR backgroundColor = al_map_rgb(0, 0, 0);
	al_clear_to_color(backgroundColor);

	mPaddleOne->Render();
	mPaddleTwo->Render();
	mScoreOne->Render();
	//mScoreTwo->Render();
	for (int i = 0; i < 10; i++) {
		mBalls[i]->Render();
	}
	al_flip_display();
}

void Game::CheckCollisions(ball* ball)
{
	if (ball->GetPosY() + ball->GetRadius() >= al_get_display_height(mDisplay) || ball->GetPosY() - ball->GetRadius() <= 0)
	{
		ball->SetMovementVector(ball->GetDirX(), ball->GetDirY() * -1);
	}

	if (ball->GetPosX() + ball->GetRadius() >= al_get_display_width(mDisplay) || ball->GetPosX() - ball->GetRadius() <= 0)
	{
		ball->SetMovementVector(ball->GetDirX() * -1, ball->GetDirY());
	}

}

void Game::Update()
{
	std::thread receiveThread(&Game::Receive,this);

	while (mRunning)
	{

		ProcessLocalInput();
		Render();

		if (mIsServer)
		{
			for (int i = 0; i < 10; i++) {
				CheckCollisions(mBalls[i]);
				CheckCollisionsPaddle(mBalls[i], mPaddleOne, mPaddleTwo);
				UpdateBall(mBalls[i]);
			}
			SendUpdatedStates();
		}
		else
		{
			SendUpdatedStates();
		}
		
	}

	receiveThread.join();
}

void Game::UpdateNetworkedPaddle(int y)
{
	Paddle* networkedPaddle;

	if (mLocalLeft)
	{
		networkedPaddle = mPaddleTwo;
	}
	else
	{
		networkedPaddle = mPaddleOne;
	}


	networkedPaddle->SetPosition(y);
}


bool Game::InitAllegro()
{
	//Init allegro
	if (!al_init())
	{
		std::cout << "error initting Allegro\n";
		system("pause");
		return false;
	}
	//Init font add on
	if (!al_init_font_addon())
	{
		std::cout << "error initting font add-on\n";
		system("pause");
		return false;
	}
	//Init ttf add on
	if (!al_init_ttf_addon())
	{
		std::cout << "error initting ttf add-on\n";
		system("pause");
		return false;
	}
	if (!al_init_primitives_addon())
	{
		std::cout << "error initting primitive add-on\n";
		system("pause");
		return false;
	}
	//Init keyboard
	if (!al_install_keyboard())
	{
		std::cout << "error installing Allegro keyboard plugin\n";
		system("pause");
		return false;
	}



	mDisplay = al_create_display(800, 640);

	return true;

}

void Game::StartServer()
{
	SocketUtil::StaticInit();


	// Create socket
	TCPSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (TCPSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}

	//listenSocket->SetNonBlockingMode(true);

	LOG("%s", "Listening socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	if (listenAddress == nullptr)
	{
		SocketUtil::ReportError("Creating listen address");
		ExitProcess(1);
	}

	if (TCPSocket->Bind(*listenAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding listening socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound listening socket");

	// Blocking function call -> Waits for some input; halts the program until something "interesting" happens
	// Non-Blocking function call -> Returns right away, as soon as the action is completed

	// Listen() - Listen on socket -> Non-blocking; tells OS we care about incoming connections on this socket
	if (TCPSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on listening socket");
		ExitProcess(1);
	}

	LOG("%s", "Listening on socket");

	// Accept() - Accept on socket -> Blocking; Waits for incoming connection and completes TCP handshake

	LOG("%s", "Waiting to accept connections...");

	TCPSocket->SetNonBlockingMode(false);

	SocketAddress incomingAddress;
	TCPSocketPtr connSocket = TCPSocket->Accept(incomingAddress);
	
	TCPSocket->~TCPSocket();
	TCPSocket = connSocket;
	TCPSocket->SetNonBlockingMode(false);
	LOG("Accepted connection from %s", incomingAddress.ToString().c_str());


}

void Game::ConnectToServer(std::string ip)
{
	SocketUtil::StaticInit();

	// Create socket
	TCPSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (TCPSocket == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
		return;
	}

	string address = "127.0.0.2:8080";
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError("Creating client address");
		ExitProcess(1);
		return;
	}

	if (TCPSocket->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding client socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound client socket");

	// Connect() -> Connect socket to remote host

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString(ip);
	if (servAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	if (TCPSocket->Connect(*servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}


	LOG("%s", "Connected to server!");

	
	TCPSocket->SetNonBlockingMode(true);

}