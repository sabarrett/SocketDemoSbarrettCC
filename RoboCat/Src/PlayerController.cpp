#include "RoboCatPCH.h"
#include "PlayerController.h"

PlayerController::PlayerController(const int networkID, GraphicsLibrary* gLib)
{
	pPressKey = new InputSystem();
	pPressKey->init(gLib);
	mNetworkID = networkID;
}

PlayerController::~PlayerController()
{
	delete pPressKey;
	pPressKey = nullptr;
}
