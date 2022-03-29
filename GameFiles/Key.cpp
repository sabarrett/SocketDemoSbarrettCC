#include "RoboCatPCH.h"
#include "Key.h"
#include "Lock.h"
#include "Coin.h"
#include "WorldState.h"

Key::Key() : GameObject("keyBlue.png", 70, 70)
{

}

Key::Key(int posX, int posY) : GameObject("keyBlue.png", posX, posY, 70, 70)
{

}

Key::~Key()
{
}

void Key::Update(WorldState* gameWorld)
{
	if (mGraphicsData.mX < 0) mGraphicsData.mX = 1500;
	mGraphicsData.mX--;

	for (int i = 0; i < gameWorld->mGameObjects.size(); i++)
	{
		if (gameWorld->mGameObjects[i]->GetClassId() == 'LOCK'
			&& (gameWorld->mGameObjects[i]->GetPosition() - GetPosition()).Length2D() < mGraphicsData.mZ)
		{

			gameWorld->Create<Coin>(mGraphicsData.mX,mGraphicsData.mY);
			gameWorld->SetForDestroy(gameWorld->mGameObjects[i]);
			gameWorld->SetForDestroy(this);
			break;
		}
	}
	
}