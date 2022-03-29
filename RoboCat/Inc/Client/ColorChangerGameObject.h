#pragma once
#include "SDL.h"
#include "Util/NetworkEnums.h"
#include "GameObject.h"

#include <vector>

class OutputMemoryBitStream;
class InputMemoryBitStream;

class ColorChangerGameObject : public GameObject {

public:
	ColorChangerGameObject() {};
	~ColorChangerGameObject() {};

	// Core functions
	void Init(int m_UID, uint8_t textureID);
	void Update(std::vector<GameObject*> collidableObjects);
	void Draw(SDL_Renderer* renderer, SDL_Texture* texture);
	void CleanUp();

	void AddTargetObject(GameObject* targetObject);
private:
	void CheckCollisions(std::vector<GameObject*> collidableObjects);

public:
	// Networking
	void Write(OutputMemoryBitStream* outStream) override;
	void Read(InputMemoryBitStream* inStream) override;

private:
	int m_colorChangeFrameDelay = 60;
	int m_colorChangeFramesPast = 0;

	std::vector<GameObject*> m_TargetObjects;
};