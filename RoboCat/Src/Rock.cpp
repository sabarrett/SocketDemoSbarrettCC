#include "Rock.h"
#include "RoboCatPCH.h"

Rock::Rock(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary)
	: GameObject(gameObjectID, networkID, graphicsLibrary)
{

}

Rock::Rock(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, const std::string spriteIdentifier)
	: GameObject(gameObjectID, networkID, graphicsLibrary, position, spriteIdentifier)
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

//void Rock::read(InputMemoryBitStream& stream)
//{
//	//stream.ReadBytes();
//}
//
//void Rock::write(OutputMemoryBitStream& stream) const
//{
//
//}