#include "Wall.h"
#include "RoboCatPCH.h"

Wall::Wall(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary)
	: GameObject(gameObjectID, networkID, graphicsLibrary)
{

}

Wall::Wall(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, float sizeX, float sizeY)
	: GameObject(gameObjectID, networkID, graphicsLibrary, position)
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