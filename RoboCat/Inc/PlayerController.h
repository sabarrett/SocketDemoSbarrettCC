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

	//int getPlayerID() { return mPlayerID; }
	//void setPlayerID(int ID) { mPlayerID = ID; }

private:
	InputSystem* pPressKey;
	bool mIsConnected;
	//int mPlayerID;
};