#include "RoboCatPCH.h"
#include "Key.h"

Key::Key() : GameObject("keyBlue.png", 70, 70)
{

}

Key::Key(int posX, int posY) : GameObject("keyBlue.png", posX, posY, 70, 70)
{

}

Key::~Key()
{
}

void Key::Update()
{
	if (mGraphicsData.mX > 1500) mGraphicsData.mX = 0;
	mGraphicsData.mX++;
}