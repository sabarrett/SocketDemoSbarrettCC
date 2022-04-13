#include "PlayerController.h"
#include "RoboCatPCH.h"

PlayerController::PlayerController(const int networkID, GraphicsLibrary* graphicsLibrary)
	: GameObject(GameObjectType::PLAYER, networkID, graphicsLibrary)
{
	//Key down
	pInputKeyDown = new InputSystem();
	pInputKeyDown->init(graphicsLibrary);

	//Key up
	pInputKeyUp = new InputSystem();
	pInputKeyUp->init(graphicsLibrary);

	mMoveSpeed = 0.0;
}

PlayerController::PlayerController(const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, float moveSpeed, const std::string spriteIdentifier)
	: GameObject(GameObjectType::PLAYER, networkID, graphicsLibrary, position, spriteIdentifier)
{
	//Key down
	pInputKeyDown = new InputSystem();
	pInputKeyDown->init(graphicsLibrary);

	//Key up
	pInputKeyUp = new InputSystem();
	pInputKeyUp->init(graphicsLibrary);

	mMoveSpeed = moveSpeed;
}

void PlayerController::HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
	//Do nothing, the data will be stale if re-sent, since it is sending update packets every frame (only latest data matters)
}

void PlayerController::HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
	//Cool, already taken care of
}

PlayerController::~PlayerController()
{
	//Key down
	delete pInputKeyDown;
	pInputKeyDown = nullptr;

	//Key up
	delete pInputKeyUp;
	pInputKeyUp = nullptr;
}

void PlayerController::update()
{
	//Get keyboard input - KeyPressed
	{
		KeyCode keyCode = pInputKeyDown->getKeyboardInput(InputMode::KeyPressed);

		switch (keyCode)
		{
		case KeyCode::W:
		{
			//Move up
			bShouldMoveUp = true;
			break;
		}

		case KeyCode::A:
		{
			//Move left
			bShouldMoveLeft = true;
			break;
		}

		case KeyCode::S:
		{
			//Move down
			bShouldMoveDown = true;
			break;
		}

		case KeyCode::D:
		{
			//Move right
			bShouldMoveRight = true;
			break;
		}

		default:
			break;
		}
	}

	//Get keyboard input - KeyReleased
	{
		KeyCode keyCode = pInputKeyUp->getKeyboardInput(InputMode::KeyReleased);

		switch (keyCode)
		{
		case KeyCode::W:
		{
			//Stop moving up
			bShouldMoveUp = false;
			break;
		}

		case KeyCode::A:
		{
			//Stop moving left
			bShouldMoveLeft = false;
			break;
		}

		case KeyCode::S:
		{
			//Stop moving down
			bShouldMoveDown = false;
			break;
		}

		case KeyCode::D:
		{
			//Stop moving right
			bShouldMoveRight = false;
			break;
		}

		default:
			break;
		}
	}

	//Apply move based on holding keys
	if (bShouldMoveUp)
		mPosition.second -= mMoveSpeed;
	if (bShouldMoveDown)
		mPosition.second += mMoveSpeed;
	if (bShouldMoveLeft)
		mPosition.first -= mMoveSpeed;
	if (bShouldMoveRight)
		mPosition.first += mMoveSpeed;
}

void PlayerController::draw()
{
	//Draw sprite at mPosition
	pGraphicsLibrary->drawScaledImage(mSPRITE_IDENTIFIER, mPosition.first, mPosition.second, 0.25, 0.25);
}