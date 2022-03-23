#include "Client/Game.h"
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
	m_isRunning = true;

	CreateObject("Player", "Assets/Player.png");
}

void Game::CreateGameWindow(char* title, int width, int height)
{
	m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	m_renderer = SDL_CreateRenderer(m_window, -1, 0);

	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
}

void Game::Update()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent) != 0)
	{
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			m_isRunning = false;
			break;
		default:
			break;
		}
	}
	m_keyStates = SDL_GetKeyboardState(NULL);

	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		m_allObjects[i]->Update(this);
	}
}

void Game::Draw()
{
	SDL_RenderClear(m_renderer);
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		m_allObjects[i]->Draw();
	}
	SDL_RenderPresent(m_renderer);
}

GameObject* Game::CreateObject(std::string name, std::string filePath)
{
	GameObject* gameObject = new GameObject;
	gameObject->Init(this, name, filePath);
	int i = 0;
	m_allObjects.push_back(gameObject);
	return gameObject;
}

void Game::DestroyObject(GameObject* gameObject)
{
	gameObject->CleanUp();
}

void Game::CleanUp()
{
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		DestroyObject(m_allObjects[i]);
	}
	SDL_DestroyWindow(m_window);
	SDL_DestroyRenderer(m_renderer);
	SDL_Quit();
}


