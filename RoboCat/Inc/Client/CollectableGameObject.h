#pragma once
#include "SDL.h"
#include "Util/NetworkEnums.h"
#include "GameObject.h"
class OutputMemoryBitStream;
class InputMemoryBitStream;

class CollectableGameObject : public GameObject {

public:
	CollectableGameObject() {};
	~CollectableGameObject() {};

	// Core functions
	void Init(int m_UID, uint8_t textureID);
	void Update(std::vector<GameObject*> collidableObjects);
	void CleanUp();

	// Get
	int getValue() { return m_value; }

	// Set
	void setValue(int value) { m_value = value; }

	// Networking
	void Write(OutputMemoryBitStream* outStream) override;
	void Read(InputMemoryBitStream* inStream) override;
private:
	int m_value = 1;
};