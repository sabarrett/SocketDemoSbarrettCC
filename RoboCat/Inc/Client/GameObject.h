#pragma once
#include "SDL.h"
#include "SDL_image.h"
class Game;
#include "Game.h"

#include <string>

class GameObject {

public:
	GameObject();
	~GameObject();

	// Core functions
	void Init(Game* game, std::string name);
	void Init(Game* game, std::string name, std::string filePath);
	void Update(Game* game);
	void Draw();
	void CleanUp();


	// Get

private:
	SDL_Renderer* m_gameRenderer;
	std::string m_name;
	SDL_Texture* m_texture;
	SDL_Rect m_textureRect;

	int m_UID = 0;

	float m_xPos = 0;
	float m_yPos = 0;

	float m_xVel = 0;
	float m_yVel = 0;
};