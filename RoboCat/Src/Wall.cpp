#include "Wall.h"
#include "RoboCatPCH.h"

Wall::Wall(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary)
	: GameObject(gameObjectID, networkID, graphicsLibrary)
{
	mSizeX = 1.0;
	mSizeY = 2.0;
}

Wall::Wall(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, float sizeX, float sizeY, const std::string spriteIdentifier)
	: GameObject(gameObjectID, networkID, graphicsLibrary, position, spriteIdentifier)
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