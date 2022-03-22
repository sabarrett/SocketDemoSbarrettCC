#pragma once

class GameObject;
#include "GameObject.h"
#include "SDL.h"
#include "SDL_image.h"
#undef main

#include <string>
#include <vector>


class Game {

public:
	Game();
	~Game();

	// Core functions
	void Init(char* title, int width, int height);
	void Update();
	void Draw();
	void CleanUp();

	GameObject* CreateObject(std::string name, std::string filePath);
	void DestroyObject(GameObject* gameObject);

	
	void CreateGameWindow(char* title, int width, int height);

	// Get
	bool Running() { return m_isRunning; };
	const Uint8* getKeyStates() { return m_keyStates; }
	SDL_Renderer* getRenderer() { return m_renderer; }
private:
	bool m_isRunning;
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	std::vector<GameObject*> m_allObjects;

	const Uint8* m_keyStates;

	int m_nextUID = 0;
};