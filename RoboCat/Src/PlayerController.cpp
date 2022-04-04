#include "RoboCatPCH.h"
#include "PlayerController.h"

PlayerController::PlayerController()
{

}

PlayerController::PlayerController(const int networkID, GraphicsLibrary* gLib)
	:GameObjects(GameObjType::PLAYER, gLib, networkID)
{
	pPressKey = new InputSystem();
	pPressKey->init(gLib);
	mPlayerID = networkID;
}

PlayerController::~PlayerController()
{
	delete pPressKey;
	pPressKey = nullptr;
}
