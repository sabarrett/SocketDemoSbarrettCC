#include "RoboCatPCH.h"
#include "Networker.h"
#include <random>

Networker::Networker():
	mDropPacketChance(0.3f),
	mSimulatedLatency(0.3f)
{
	gameData = GameData::getInstance();
}

Networker::~Networker()
{

}

float Networker::GetRandomFloat()
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_real_distribution< float > dis(0.f, 1.f);
	return dis(gen);
}

void Networker::SetSocket(TCPSocketPtr newSocket)
{
	inSocket = newSocket;
}

//void Networker::SetFromAddress(SocketAddress newFromAddress)
//{
//	mFromAddress = newFromAddress;
//}

GameObjectType Networker::ReceivePacket(int& currentMaxID, bool& isGameRunning)
{
	GameObjectType type = GameObjectType::ENUM_SIZE;
	int id = 0;
	float x = 0;
	float y = 0;

	char buffer[1500];
	int32_t bytesReceived = inSocket->Receive(buffer, 1500);

	/*if (bytesReceived == 0)
	{
		LOG("%s", " Connection gracefully closed. Press enter to exit!");
		isGameRunning = false;
	}*/
	//if (bytesReceived < 0)
	//{
	//	LOG("%s", " Connection forcefully closed. Press enter to exit!");
	//	isGameRunning = false;
	//}
	if (bytesReceived > 0)
	{
		InputMemoryBitStream stream(buffer, 1500);
		
		stream.Read(type);

		if (type == GameObjectType::END_OF_GAME)
		{
			isGameRunning = false;
		}
		else
		{
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
					bullet->pDeliveryNotificationManager = pDeliveryNotificationManager;
					gameData->bulletsVector.push_back(bullet);
					//std::cout << "Bullet spawn 6" << std::endl;
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
					effect->pDeliveryNotificationManager = pDeliveryNotificationManager;
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

					gameData->player2->MoveTo(-1 * x + RESOLUTION_X);
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
								if (gotDestroyed)
								{
									delete bullet;
									bullet = nullptr;
									gameData->bulletsVector.erase(std::remove(gameData->bulletsVector.begin(), gameData->bulletsVector.end(), nullptr), gameData->bulletsVector.end());
								}
								else
								{
									//std::cout << "Bullet spawn 7" << std::endl;
									bullet->setX(-1 * x + RESOLUTION_X);
									bullet->setY(-1 * y + RESOLUTION_Y);
									bullet->gotDestroyed = false;
								}
							}
						}
					}

					break;
				}
				}
			}
		}

	}
	return type;
}

void Networker::ReceivePacketIntoQueue()
{
	char packetMem[1500];
	int packetSize = sizeof(packetMem);
	InputMemoryBitStream inputStream(packetMem, packetSize * 8);

	//keep reading until we don't have anything to read ( or we hit a max number that we'll process per frame )
	int receivedPackedCount = 0;
	int totalReadByteCount = 0;

	while (receivedPackedCount < kMaxPacketsPerFrameCount)
	{
		int readByteCount = inSocket->Receive(packetMem, packetSize);
		if (readByteCount == 0)
		{
			//nothing to read
			break;
		}
		else if (readByteCount > 0)
		{
			inputStream.ResetToCapacity(readByteCount);
			++receivedPackedCount;
			totalReadByteCount += readByteCount;

			//now, should we drop the packet?
			if (GetRandomFloat() >= mDropPacketChance)
			{
				//we made it
				//shove the packet into the queue and we'll handle it as soon as we should...
				//we'll pretend it wasn't received until simulated latency from now
				//this doesn't sim jitter, for that we would need to.....

				float simulatedReceivedTime = Timing::sInstance.GetTimef() + mSimulatedLatency;
				mPacketQueue.emplace(simulatedReceivedTime, inputStream);
			}
			else
			{
				LOG("Dropped packet!", 0);
				//dropped!
			}
		}
		else
		{
			//uhoh, error? exit or just keep going?
		}
	}
}

GameObjectType Networker::ProcessQueuedPackets(int& currentMaxID, bool& isGameRunning)
{
	//look at the front packet...
	while (!mPacketQueue.empty())
	{
		ReceivedPacket& nextPacket = mPacketQueue.front();
		if (Timing::sInstance.GetTimef() > nextPacket.GetReceivedTime())
		{
			bool isCorrect = pDeliveryNotificationManager->ReadAndProcessState(nextPacket.GetPacketBuffer());

			GameObjectType type = GameObjectType::ENUM_SIZE;
			nextPacket.GetPacketBuffer().Read(type);
			int id = 0;
			float x = 0;
			float y = 0;

			if (isCorrect || type == GameObjectType::EFFECT)
			{
				if (type == GameObjectType::END_OF_GAME)
				{
					isGameRunning = false;
				}
				else
				{
					nextPacket.GetPacketBuffer().Read(id);

					if (id >= currentMaxID)
					{
						switch (type)
						{
						case GameObjectType::BULLET:
						{
							bool isGoingUpwards;

							nextPacket.GetPacketBuffer().Read(x);
							nextPacket.GetPacketBuffer().Read(y);
							nextPacket.GetPacketBuffer().Read(isGoingUpwards);

							Bullet* bullet = new Bullet(id, -1 * x + RESOLUTION_X, -1 * y + RESOLUTION_Y, BULLET_SPEED, !isGoingUpwards);
							bullet->pDeliveryNotificationManager = pDeliveryNotificationManager;
							gameData->bulletsVector.push_back(bullet);
							//std::cout << "Bullet spawn 6" << std::endl;
							currentMaxID = id + 1;
							break;
						}
						case GameObjectType::EFFECT:
						{
							bool shouldDisplay;

							nextPacket.GetPacketBuffer().Read(x);
							nextPacket.GetPacketBuffer().Read(y);
							nextPacket.GetPacketBuffer().Read(shouldDisplay);

							Effect* effect = new Effect(id, -1 * x + RESOLUTION_X, -1 * y + RESOLUTION_Y, shouldDisplay);
							effect->pDeliveryNotificationManager = pDeliveryNotificationManager;
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

							nextPacket.GetPacketBuffer().Read(x);
							nextPacket.GetPacketBuffer().Read(y);
							nextPacket.GetPacketBuffer().Read(isFiring);
							nextPacket.GetPacketBuffer().Read(isHit);

							gameData->player2->MoveTo(-1 * x + RESOLUTION_X);
							gameData->player2->SetIsFiring(isFiring);
							gameData->player2->SetIsHit(isHit);

							break;
						}
						case GameObjectType::BULLET:
						{
							bool isMovingUpwards;
							bool gotDestroyed;
							nextPacket.GetPacketBuffer().Read(x);
							nextPacket.GetPacketBuffer().Read(y);
							nextPacket.GetPacketBuffer().Read(isMovingUpwards);
							nextPacket.GetPacketBuffer().Read(gotDestroyed);


							for (auto& bullet : gameData->bulletsVector)
							{
								if (bullet != nullptr)
								{
									if (bullet->getGameID() == id)
									{
										if (gotDestroyed)
										{
											delete bullet;
											bullet = nullptr;
											gameData->bulletsVector.erase(std::remove(gameData->bulletsVector.begin(), gameData->bulletsVector.end(), nullptr), gameData->bulletsVector.end());
										}
										else
										{
											//std::cout << "Bullet spawn 7" << std::endl;
											bullet->setX(-1 * x + RESOLUTION_X);
											bullet->setY(-1 * y + RESOLUTION_Y);
											bullet->gotDestroyed = false;
										}
									}
								}
							}

							break;
						}
						}
					}
				}
			}

			mPacketQueue.pop();
			return type;
		}
		else
		{
			break;
		}

	}

}

Networker::ReceivedPacket::ReceivedPacket(float inReceivedTime, InputMemoryBitStream& ioInputMemoryBitStream) :
	mReceivedTime(inReceivedTime),
	mPacketBuffer(ioInputMemoryBitStream)
{
}