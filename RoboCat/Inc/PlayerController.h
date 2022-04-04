#pragma once

#include "GameObjects.h"
#include "InputSystem.h"

class PlayerController : public GameObjects
{
public:

	PlayerController();
	PlayerController(const int networkID, GraphicsLibrary* gLib);
	~PlayerController();

	void update() {};
	void draw() {};

	void setIsConnected(bool connected) { mIsConnected = connected; }

private:
	InputSystem* pPressKey;
	bool mIsConnected;
};