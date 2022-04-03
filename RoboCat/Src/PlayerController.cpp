#include "RoboCatPCH.h"
#include "PlayerController.h"

PlayerController::PlayerController()
{
	//networkID = 0;
}

PlayerController::PlayerController(const int networkID, GraphicsLibrary* gLib)
	:GameObjects(CurrentObject::PLAYER, gLib, networkID)
{
	pPressKey = new InputSystem();
	pPressKey->init(gLib);
	//mNetworkID = networkID;
}

PlayerController::~PlayerController()
{
	delete pPressKey;
	pPressKey = nullptr;
}
