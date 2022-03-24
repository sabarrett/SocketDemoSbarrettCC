#include "Wall.h"
#include "RoboCatPCH.h"

Wall::Wall(const int gameObjectID, const int networkID)
	: GameObject(gameObjectID, networkID)
{

}

Wall::Wall(const int gameObjectID, const int networkID, float posX, float posY, float sizeX, float sizeY)
	: GameObject(gameObjectID, networkID, posX, posY)
{
	mSizeX = sizeX;
	mSizeY = sizeY;
}

Wall::~Wall()
{

}