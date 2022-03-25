#include "Rock.h"
#include "RoboCatPCH.h"

Rock::Rock(const int gameObjectID, const int networkID)
	: GameObject(gameObjectID, networkID)
{

}

Rock::Rock(const int gameObjectID, const int networkID, float posX, float posY)
	: GameObject(gameObjectID, networkID, posX, posY)
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