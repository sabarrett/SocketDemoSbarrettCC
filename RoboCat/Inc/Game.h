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
	bool Running() { return isRunning; };
	const Uint8* getKeyStates() { return m_keyStates; }
private:
	bool isRunning;
	SDL_Window* window;
	SDL_Renderer* renderer;
	std::vector<GameObject*> allObjects;

	const Uint8* m_keyStates;
};