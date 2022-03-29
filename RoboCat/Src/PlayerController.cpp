#include "PlayerController.h"
#include "RoboCatPCH.h"

PlayerController::PlayerController(const int gameObjectID, const int networkID, InputSystem* inputSystem)
	: GameObject(gameObjectID, networkID)
{
	pInput = inputSystem;
	mMoveSpeed = 0.0;
}

PlayerController::PlayerController(const int gameObjectID, const int networkID, pair<float, float> position, float moveSpeed, InputSystem* inputSystem)
	: GameObject(gameObjectID, networkID, position)
{
	pInput = inputSystem;
	mMoveSpeed = moveSpeed;
}

PlayerController::~PlayerController()
{
	//This gets deleted in main.cpp, only de-reference it here
	pInput = nullptr;
}

void PlayerController::update()
{
	//Get keyboard input
	{
		KeyCode keyCode = pInput->getKeyboardInput();

		switch (keyCode)
		{
		case KeyCode::W:
		{
			//Move up
			mPosition.first -= mMoveSpeed;

			break;
		}

		case KeyCode::A:
		{
			//Move left
			mPosition.second -= mMoveSpeed;

			break;
		}

		case KeyCode::S:
		{
			//Move down
			mPosition.first += mMoveSpeed;

			break;
		}

		case KeyCode::D:
		{
			//Move right
			mPosition.second += mMoveSpeed;

			break;
		}

		default:
			break;
		}
	}
}

void PlayerController::draw()
{

}