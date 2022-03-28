#include "Wall.h"
#include "RoboCatPCH.h"

Wall::Wall(const int gameObjectID, const int networkID)
	: GameObject(gameObjectID, networkID)
{

}

Wall::Wall(const int gameObjectID, const int networkID, pair<float, float> position, float sizeX, float sizeY)
	: GameObject(gameObjectID, networkID, position)
{
	mSizeX = sizeX;
	mSizeY = sizeY;
}

Wall::~Wall()
{

}

void Wall::update()
{

}

void Wall::draw()
{

}