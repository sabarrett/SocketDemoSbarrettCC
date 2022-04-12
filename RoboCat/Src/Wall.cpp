#include "Wall.h"
#include "RoboCatPCH.h"

Wall::Wall(const int networkID, GraphicsLibrary* graphicsLibrary)
	: GameObject(GameObjectType::WALL, networkID, graphicsLibrary)
{
	mSizeX = 1.0;
	mSizeY = 2.0;

	mColour = Colour(255, 255, 255, 255);
	mThickness = 1.0;
}

Wall::Wall(const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, float sizeX, float sizeY, Colour colour, float thickness)
	: GameObject(GameObjectType::WALL, networkID, graphicsLibrary, position)
{
	mSizeX = sizeX;
	mSizeY = sizeY;

	mColour = colour;
	mThickness = thickness;
}

void Wall::HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
	inDeliveryNotificationManager->
}

void Wall::HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
	inDeliveryNotificationManager->
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