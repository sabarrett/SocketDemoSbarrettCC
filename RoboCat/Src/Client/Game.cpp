#include "Client/Game.h"
#include "Client/GameObject.h"
#include "Client/ColorChangerGameObject.h"
#include "Client/CollectableGameObject.h"

#include <iostream>

void Game::Init(char* title, int width, int height)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	CreateGameWindow(title, width, height);
	m_isRunning = true;

	m_networkManager.Init(this);

	LoadTexture("Assets/Player.png");
	LoadTexture("Assets/PlayerScheming.png");
	LoadTexture("Assets/PlayerRed.png");
	LoadTexture("Assets/PlayerGreen.png");
	LoadTexture("Assets/PlayerBlue.png");
	LoadTexture("Assets/Cat.png");
	LoadTexture("Assets/Coin.png");
	LoadTexture("Assets/Background.png");
}
void Game::CreateGameWindow(char* title, int width, int height)
{
	m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	m_renderer = SDL_CreateRenderer(m_window, -1, 0);

	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
}

void Game::LoadTexture(std::string assetPath)
{
	int i = 0;
	while (m_textures[i] != 0)
	{
		i++;
	}
	SDL_Surface* tmpSurface = IMG_Load(assetPath.c_str());
	m_textures[i] = SDL_CreateTextureFromSurface(m_renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);
}

void Game::ConnectToServer(std::string address, std::string port)
{
	m_networkManager.ConnectToServer(address, port);
}

void Game::Update()
{
	// Handle events
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

	// Handle keyboard input
	m_input.m_keyStates = SDL_GetKeyboardState(NULL);
	m_input.m_moveInput = 0;
	if (m_input.m_keyStates[SDL_SCANCODE_W])
	{
		m_input.m_moveInput += MOVE_UP;
	}
	if (m_input.m_keyStates[SDL_SCANCODE_S])
	{
		m_input.m_moveInput += MOVE_DOWN;
	}
	if (m_input.m_keyStates[SDL_SCANCODE_A])
	{
		m_input.m_moveInput += MOVE_LEFT;
	}
	if (m_input.m_keyStates[SDL_SCANCODE_D])
	{
		m_input.m_moveInput += MOVE_RIGHT;
	}
	
	// Handle mouse input
	m_input.m_leftClickDown = false;
	if (SDL_GetMouseState(&m_input.m_mouseX, &m_input.m_mouseY) == SDL_BUTTON_LMASK)
	{
		m_input.m_leftClickDown = true;
	}
	


	m_networkManager.ProcessIncomingPackets();
	m_networkManager.SendOutgoingPackets();
}

void Game::Draw()
{
	// Clear + background
	SDL_RenderClear(m_renderer);
	SDL_RenderCopy(m_renderer, m_textures[7], NULL, NULL);

	// Draw all objects
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		int texID = m_allObjects[i]->GetTextureID();
		m_allObjects[i]->Draw(m_renderer, m_textures[texID]);
	}
	SDL_RenderPresent(m_renderer);
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


