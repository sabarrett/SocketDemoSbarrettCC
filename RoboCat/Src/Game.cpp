#include "Game.h"
#include <iostream>
Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(char* title, int width, int height)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	CreateGameWindow(title, width, height);
	isRunning = true;

	CreateObject("Player", "Assets/player.png");
}

void Game::CreateGameWindow(char* title, int width, int height)
{
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, 0);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	

	
}

void Game::Update()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent) != 0)
	{
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		default:
			break;
		}
	}
	m_keyStates = SDL_GetKeyboardState(NULL);

	for (size_t i = 0; i < allObjects.size(); i++)
	{
		allObjects[i]->Update(this);
	}
	
}

void Game::Draw()
{
	SDL_RenderClear(renderer);
	for (size_t i = 0; i < allObjects.size(); i++)
	{
		allObjects[i]->Draw();
	}
	SDL_RenderPresent(renderer);
}

GameObject* Game::CreateObject(std::string name, std::string filePath)
{
	GameObject* gameObject = new GameObject;
	gameObject->Init(renderer, name, filePath);
	allObjects.push_back(gameObject);
	return gameObject;
}

void Game::DestroyObject(GameObject* gameObject)
{
	gameObject->CleanUp();
}

void Game::CleanUp()
{
	for (size_t i = 0; i < allObjects.size(); i++)
	{
		DestroyObject(allObjects[i]);
	}
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
}


