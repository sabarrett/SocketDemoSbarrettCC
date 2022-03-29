#include "Rock.h"
#include "RoboCatPCH.h"

Rock::Rock(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary)
	: GameObject(gameObjectID, networkID, graphicsLibrary)
{

}

Rock::Rock(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, const std::string spriteIdentifier)
	: GameObject(gameObjectID, networkID, position, graphicsLibrary, spriteIdentifier)
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