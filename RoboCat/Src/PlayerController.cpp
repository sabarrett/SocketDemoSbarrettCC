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
	//Get keyboard input - KeyPressed
	{
		KeyCode keyCode = pInput->getKeyboardInput(/*InputMode::KeyPressed*/);

		switch (keyCode)
		{
		case KeyCode::W:
		{
			//Move up
			mPosition.second -= mMoveSpeed;

			break;
		}

		case KeyCode::A:
		{
			//Move left
			mPosition.first -= mMoveSpeed;

			break;
		}

		case KeyCode::S:
		{
			//Move down
			mPosition.second += mMoveSpeed;

			break;
		}

		case KeyCode::D:
		{
			//Move right
			mPosition.first += mMoveSpeed;

			break;
		}

		default:
			break;
		}
	}

	////Get keyboard input - KeyReleased
	//{
	//	KeyCode keyCode = pInput->getKeyboardInput(InputMode::KeyReleased);

	//	switch (keyCode)
	//	{
	//	case KeyCode::W:
	//	{
	//		//Move up
	//		bShouldMoveUp = false;

	//		break;
	//	}

	//	case KeyCode::A:
	//	{
	//		//Move left
	//		bShouldMoveLeft = false;

	//		break;
	//	}

	//	case KeyCode::S:
	//	{
	//		//Move down
	//		bShouldMoveDown = false;

	//		break;
	//	}

	//	case KeyCode::D:
	//	{
	//		//Move right
	//		bShouldMoveRight = false;

	//		break;
	//	}

	//	default:
	//		break;
	//	}
	//}

	//if (bShouldMoveUp)
	//	mPosition.second -= mMoveSpeed;

	//if (bShouldMoveDown)
	//	mPosition.second += mMoveSpeed;

	//if (bShouldMoveLeft)
	//	mPosition.first -= mMoveSpeed;

	//if (bShouldMoveRight)
	//	mPosition.first += mMoveSpeed;
}

void PlayerController::draw()
{
	//Draw sprite at mPosition
	pGraphicsLibrary->drawImage(mSPRITE_IDENTIFIER, mPosition.first, mPosition.second);
}