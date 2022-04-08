#include "Game.h"

Game::Game()
{
	mpGraphicsLib = new GraphicsLib(SCREEN_SIZE_X, SCREEN_SIZE_Y);
	mpGraphicsLib2 = new GraphicsLib(SCREEN_SIZE_X, SCREEN_SIZE_Y);

	init();
}

Game::~Game()
{
}

void Game::init()
{
	mpInputSystem = new InputSystem();

	mpGraphicsLib->loadImage(FILE_PATH + BACKGROUND_FILE, "background");
	mpGraphicsLib->loadImage(FILE_PATH + HOMER_FILE, "homer");
	mpGraphicsLib->drawImage("background", 0, 0);
	mpGraphicsLib->drawImage("homer", 0, 100);
	mpGraphicsLib->render();

	mpGraphicsLib->loadImage(FILE_PATH + BACKGROUND_FILE, "background");
	mpGraphicsLib->loadImage(FILE_PATH + QUIMBY_FILE, "quimby");
	mpGraphicsLib->drawImage("background", 0, 0);
	mpGraphicsLib->drawImage("quimby", 1500, 100);
	mpGraphicsLib->render();

	isPlaying = true;
}

void Game::cleanup()
{

}

void Game::gameLoop()
{
	while (isPlaying == true)
	{
		input();
		update();
		render();
	}
}

void Game::input()
{
	switch (mpInputSystem->getKeyboardInput())
	{
	case KeyCode::Esc:
		isPlaying = false;
		break;

	case KeyCode::W:

	case KeyCode::S:

	case KeyCode::D:

	case KeyCode::A:

	case KeyCode::E:

	default:
		break;
	}
}

void Game::update()
{

}

void Game::render()
{
	mpGraphicsLib->drawImage("background", 0, 0);
	mpGraphicsLib->render();

	mpGraphicsLib2->drawImage("background", 0, 0);
	mpGraphicsLib2->render();
}
