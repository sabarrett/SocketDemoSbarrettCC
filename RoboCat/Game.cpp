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
}

void Game::cleanup()
{
}

void Game::gameLoop()
{
}

void Game::input()
{
}

void Game::update()
{
}

void Game::render()
{
	mpGraphicsLib->render();
	mpGraphicsLib2->render();
}
