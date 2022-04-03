#pragma once

#include "GameObjects.h"
#include "InputSystem.h"
#include "GameController.h"

class PlayerController : public GameObjects
{
public:

	PlayerController() { networkID = 0; };
	PlayerController(const int networkID, GraphicsLibrary* gLib);
	~PlayerController();

	void update() {};
	void draw() {};

	int getNetworkID() { return networkID; }

private:
	InputSystem* pPressKey;
	int networkID;

};