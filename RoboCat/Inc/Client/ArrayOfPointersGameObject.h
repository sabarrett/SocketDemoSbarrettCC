#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include "Util/NetworkEnums.h"
#include "GameObject.h"
class OutputMemoryBitStream;
class InputMemoryBitStream;

#include <iostream>
#include <string>
#include <vector>

class ArrayOfPointersGameObject : public GameObject {

public:
	ArrayOfPointersGameObject();
	~ArrayOfPointersGameObject();

	// Core functions
	void Init(int m_UID, uint8_t textureID);
	void Update(std::vector<GameObject*> collidableObjects);
	void CleanUp();

	bool CheckCollisions(std::vector<GameObject*> collidableObjects);
	void AddTargetObject(GameObject* targetObject);

	void Write(OutputMemoryBitStream* outStream) override;
	void Read(InputMemoryBitStream* inStream) override;
private:
	bool m_ObjectColliding = false;
	std::vector<GameObject*> m_TargetObjects;
};