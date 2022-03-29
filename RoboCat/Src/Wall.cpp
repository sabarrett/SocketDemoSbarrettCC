#include "Wall.h"
#include "RoboCatPCH.h"

Wall::Wall(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary)
	: GameObject(gameObjectID, networkID, graphicsLibrary)
{
	mSizeX = 1.0;
	mSizeY = 2.0;

	mColour = Colour(255, 255, 255, 255);
	mThickness = 1.0;
}

Wall::Wall(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, float sizeX, float sizeY, Colour colour, float thickness)
	: GameObject(gameObjectID, networkID, graphicsLibrary, position)
{
	mSizeX = sizeX;
	mSizeY = sizeY;

	mColour = colour;
	mThickness = thickness;
}

Wall::~Wall()
{

}

void Wall::update()
{

}

void Wall::draw()
{
	//Draw scaled sprite at mPosition
	pGraphicsLibrary->drawRectangle(mPosition.first, mPosition.second, mPosition.first + mSizeX, mPosition.second + mSizeY, mColour, mThickness);
}

//void Wall::read(InputMemoryBitStream& stream)
//{
//
//}
//
//void Wall::write(OutputMemoryBitStream& stream) const
//{
//
//}