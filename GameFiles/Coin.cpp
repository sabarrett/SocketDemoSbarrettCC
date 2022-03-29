#include "RoboCatPCH.h"
#include "Coin.h"

Coin::Coin() : GameObject("coinGold.png", 70, 70)
{

}

Coin::Coin(int posX, int posY) : GameObject("coinGold.png", posX, posY, 70, 70)
{

}

Coin::~Coin()
{
}

void Coin::Update(WorldState* gameWorld)
{
	//if (mGraphicsData.mX > 1500) mGraphicsData.mX = 0;
	//mGraphicsData.mX++;
}