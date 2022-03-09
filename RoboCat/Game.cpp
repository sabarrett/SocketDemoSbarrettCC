#include "Game.h"
#include "Paddle.h"
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

Game::Game()
{
	

	if (InitAllegro())
	{
		ALLEGRO_COLOR paddleColor = al_map_rgb(255, 255, 255);
		mPaddleOne = new Paddle(20, 100, paddleColor);
		mPaddleOne->SetPosition(30, 100);

		mPaddleTwo = new Paddle(20, 100, paddleColor);
		mPaddleTwo->SetPosition(al_get_display_width(mDisplay) - 50, 100);

		keyboardState = new ALLEGRO_KEYBOARD_STATE();
		mRunning = true;
	}

	mIsServer = true;
	mLocalLeft = mIsServer;

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
		mRunning = true;
	}
	mIsServer = false;
	mLocalLeft = mIsServer;

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
			localPaddle->SetPosition(localPaddle->GetPosX(), height - 2);
		}
	}

	if (moveState == -1)
	{
		if (height + localPaddle->GetDimsW() < al_get_display_height(mDisplay))
		{
			localPaddle->SetPosition(localPaddle->GetPosX(), height + 2);
		}
	}
}

void Game::CheckCollisions()
{

}

void Game::Render()
{
	ALLEGRO_COLOR backgroundColor = al_map_rgb(0, 0, 0);
	al_clear_to_color(backgroundColor);

	mPaddleOne->Render();
	mPaddleTwo->Render();

	al_flip_display();
}

void Game::Update()
{
	while (mRunning)
	{

		ProcessLocalInput();
		Render();

		if (mIsServer)
		{
			CheckCollisions();
			//SendUpdatedStates();
		}
		else
		{
			//SendUpdatedStates();
		}
		
	}


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