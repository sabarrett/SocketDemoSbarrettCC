#include "Rock.h"
#include "RoboCatPCH.h"

Rock::Rock(const int networkID, GraphicsLibrary* graphicsLibrary)
	: GameObject(GameObjectType::ROCK, networkID, graphicsLibrary)
{

}

Rock::Rock(const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, const std::string spriteIdentifier)
	: GameObject(GameObjectType::ROCK, networkID, graphicsLibrary, position, spriteIdentifier)
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
	//Draw sprite at mPosition
	pGraphicsLibrary->drawImage(mSPRITE_IDENTIFIER, mPosition.first, mPosition.second);
}