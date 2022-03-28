#include "Rock.h"
#include "RoboCatPCH.h"

Rock::Rock(const int gameObjectID, const int networkID)
	: GameObject(gameObjectID, networkID)
{

}

Rock::Rock(const int gameObjectID, const int networkID, pair<float, float> position)
	: GameObject(gameObjectID, networkID, position)
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