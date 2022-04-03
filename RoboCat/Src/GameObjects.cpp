#include "RoboCatPCH.h"

#include "GameObjects.h"

GameObjects::GameObjects(CurrentObject type, GraphicsLibrary* gLib, const int networkID)
	:mObjType(type), mNetworkID(networkID)
{
	mGraphicsLib = gLib;

	mPosX = 0.0;
	mPosY = 0.0;
}

GameObjects::GameObjects(CurrentObject type, GraphicsLibrary* gLib, const int networkID, float posX, float posY, const std::string imgID)
	:mObjType(type), mNetworkID(networkID), mIMG_ID(imgID)
{
	mGraphicsLib = gLib;

	mPosX = posX;
	mPosY = posY;
}

GameObjects::~GameObjects()
{
	mGraphicsLib = nullptr;
}

//---------------------Boulder-------------------/
Boulder::Boulder(GraphicsLibrary* gLib, const int networkID)
	:GameObjects(CurrentObject::BOULDER, gLib, networkID)
{

}

Boulder::Boulder(GraphicsLibrary* gLib, const int networkID, const std::string imgID, float posX, float posY)
	: GameObjects(CurrentObject::BOULDER, gLib, networkID, posX, posY, imgID)
{

}

void Boulder::draw()
{
	mGraphicsLib->drawScaledImage(mIMG_ID, mPosX, mPosY, 0.5, 0.5);
}

//---------------------Bubble-------------------/
Bubble::Bubble(GraphicsLibrary* gLib, const int networkID, const std::string imgID, float posX, float posY)
	: GameObjects(CurrentObject::BUBBLE, gLib, networkID, posX, posY, imgID)
{

}

void Bubble::draw()
{
	mGraphicsLib->drawScaledImage(mIMG_ID, mPosX, mPosY, 0.25, 0.25);
}


//---------------------Bees-------------------/
Bees::Bees(GraphicsLibrary* gLib, const int networkID, const std::string imgID, float posX, float posY)
	: GameObjects(CurrentObject::BEE, gLib, networkID, posX, posY, imgID)
{

}

void Bees::draw()
{
	mGraphicsLib->drawScaledImage(mIMG_ID, mPosX, mPosY, 0.25, 0.25);
}

