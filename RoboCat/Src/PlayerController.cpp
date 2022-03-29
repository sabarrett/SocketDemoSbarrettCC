#include "PlayerController.h"
#include "RoboCatPCH.h"

PlayerController::PlayerController(const int gameObjectID, const int networkID, InputSystem* inputSystem, GraphicsLibrary* graphicsLibrary)
	: GameObject(gameObjectID, networkID, graphicsLibrary)
{
	pInput = inputSystem;
	mMoveSpeed = 0.0;
}

PlayerController::PlayerController(const int gameObjectID, const int networkID, InputSystem* inputSystem, GraphicsLibrary* graphicsLibrary, pair<float, float> position, float moveSpeed, const std::string spriteIdentifier)
	: GameObject(gameObjectID, networkID, graphicsLibrary, position, spriteIdentifier)
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
	//Draw sprite at mPosition
	pGraphicsLibrary->drawImage(mSPRITE_IDENTIFIER, mPosition.first, mPosition.second);
}