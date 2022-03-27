#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "Util/NetworkEnums.h"
class OutputMemoryBitStream;
class InputMemoryBitStream;
//#include "Game.h"

#include <iostream>
#include <string>
#include <vector>

class GameObject {

public:
	GameObject();
	~GameObject();

	// Core functions
	void Init(int UID, uint8_t textureID);
	void Update(std::vector<GameObject*> collidableObjects);
	void Draw(SDL_Renderer* renderer, SDL_Texture* texture);
	void CleanUp();

	void AddVelocityFromInput(int moveInput);
	void SetPosition(float x, float y) { m_xPos = x; m_yPos = y; containsUpdatedInfo = true; }

	// Get
	int GetTextureID() { return m_textureID; };
	void SetTextureID(uint8_t textureID) { m_textureID = textureID; };
	int GetUID() { return m_UID; };
	bool GetShouldDestroy() { return m_shouldDestroy; };
	void SetShouldDestroy(bool shouldDestroy) { m_shouldDestroy = shouldDestroy; };
	float GetPosX() { return m_xPos; };
	float GetPosY() { return m_yPos; };

	virtual void Write(OutputMemoryBitStream* outStream);
	virtual void Read(InputMemoryBitStream* inStream);
protected:
	uint8_t objectType;
	bool containsUpdatedInfo = false;
	SDL_Rect m_textureRect;
	bool m_shouldDestroy = true;

	uint8_t m_textureID;
	int m_UID = 0;

	float m_xPos = 0;
	float m_yPos = 0;

	float m_xVel = 0;
	float m_yVel = 0;
};