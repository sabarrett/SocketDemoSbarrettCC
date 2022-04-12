#include "RoboCatPCH.h"
#include "Networker.h"

Networker::Networker()
{
	gameData = gameData->getInstance();
}

Networker::~Networker()
{

}

void Networker::SetSocket(TCPSocketPtr newSocket)
{
	inSocket = newSocket;
}

GameObjectType Networker::ReceivePackage(int& currentMaxID, bool& isGameRunning)
{
	GameObjectType type = GameObjectType::ENUM_SIZE;
	int id = 0;
	float x = 0;
	float y = 0;

	char buffer[128];
	int32_t bytesReceived = inSocket->Receive(buffer, 128);

	if (bytesReceived == 0)
	{
		LOG("%s", " Connection gracefully closed. Press enter to exit!");
		isGameRunning = false;
	}
	if (bytesReceived < 0)
	{
		LOG("%s", " Connection forcefully closed. Press enter to exit!");
		isGameRunning = false;
	}
	if (bytesReceived > 0)
	{
		InputMemoryBitStream stream(buffer, 128);
		stream.Read(type);
		stream.Read(id);

		if (id >= currentMaxID)
		{
			switch (type)
			{
			case GameObjectType::BULLET:
			{
				bool isGoingUpwards;

				stream.Read(x);
				stream.Read(y);
				stream.Read(isGoingUpwards);

				Bullet* bullet = new Bullet(id, -1 * x + RESOLUTION_X, -1 * y + RESOLUTION_Y, BULLET_SPEED, !isGoingUpwards);
				gameData->bulletsVector.push_back(bullet);
				currentMaxID = id + 1;
				break;
			}
			case GameObjectType::EFFECT:
			{
				bool shouldDisplay;

				stream.Read(x);
				stream.Read(y);
				stream.Read(shouldDisplay);

				Effect* effect = new Effect(id, -1 * x + RESOLUTION_X, -1 * y + RESOLUTION_Y, shouldDisplay);
				gameData->effectsVector.push_back(effect);
				currentMaxID = id + 1;
				break;
			}
			}
		}
		else
		{
			switch (type)
			{
			case GameObjectType::PLAYER:
			{
				bool isFiring;
				bool isHit;

				stream.Read(x);
				stream.Read(y);
				stream.Read(isFiring);
				stream.Read(isHit);

				gameData->player2->mPosX = (-1 * x + RESOLUTION_X);
				gameData->player2->SetIsFiring(isFiring);
				gameData->player2->SetIsHit(isHit);

				break;
			}
			case GameObjectType::BULLET:
			{
				bool isMovingUpwards;
				bool gotDestroyed;
				stream.Read(x);
				stream.Read(y);
				stream.Read(isMovingUpwards);
				stream.Read(gotDestroyed);


				for (auto& bullet : gameData->bulletsVector)
				{
					if (bullet != nullptr)
					{
						if (bullet->getGameID() == id)
						{
							bullet->mPosX = (-1 * x + RESOLUTION_X);
							bullet->mPosY = (-1 * y + RESOLUTION_Y);
							bullet->gotDestroyed = gotDestroyed;
						}
					}
				}

				break;
			}
			}
		}

		return type;
	}
}