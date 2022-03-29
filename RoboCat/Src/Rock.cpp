#include "Rock.h"
#include "RoboCatPCH.h"

Rock::Rock(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary)
	: GameObject(gameObjectID, networkID, graphicsLibrary)
{

}

Rock::Rock(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position)
	: GameObject(gameObjectID, networkID, position, graphicsLibrary)
{

}

Rock::~Rock()
{

}

void Rock::update()
{

}

void Rock::draw()
{

}