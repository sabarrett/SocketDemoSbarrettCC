#pragma once
#include "SDL.h"
#include "Util/NetworkEnums.h"

#include <vector>

class OutputMemoryBitStream;
class InputMemoryBitStream;

class GameObject {

public:
	GameObject() {};
	~GameObject() {};

	// Core functions
	virtual void Init(int UID, uint8_t textureID);
	virtual void Update(std::vector<GameObject*> collidableObjects);
	virtual void Draw(SDL_Renderer* renderer, SDL_Texture* texture);
	virtual void CleanUp();

	void AddVelocityFromInput(uint8_t moveInput);
	void SetPosition(float x, float y) { m_xPos = x; m_yPos = y; m_containsUpdatedInfo = true; };

	// Get
	int GetUID() { return m_UID; };
	int GetTextureID() { return m_textureID; };
	float GetPosX() { return m_xPos; };
	float GetPosY() { return m_yPos; };
	bool GetContainsUpdatedInfo() { return m_containsUpdatedInfo; };
	bool GetShouldDestroy() { return m_shouldDestroy; };
	uint8_t GetType() { return m_objectType; }

	// Set
	void SetTextureID(uint8_t textureID) { m_textureID = textureID; m_containsUpdatedInfo = true; };
	void SetShouldDestroy(bool shouldDestroy) { m_shouldDestroy = shouldDestroy; };
	void ForceSendUpdate() { m_containsUpdatedInfo = true; };

	// Networking
	virtual void Write(OutputMemoryBitStream* outStream);
	virtual void Read(InputMemoryBitStream* inStream);

protected:
	uint8_t m_objectType; // Type of object, for replication purposes.
	uint8_t m_textureID; // Index of texture to use for self when drawing

	int m_UID = 0; // Unique Identifier

	bool m_containsUpdatedInfo = false; // Tells whether gameObject has been updated and needs to be sent in full or if data can be left out
	bool m_shouldDestroy = false; // Object wants to/should be destroyed

	// Physics
	float m_xPos = 0;
	float m_yPos = 0;
	float m_xVel = 0;
	float m_yVel = 0;
};