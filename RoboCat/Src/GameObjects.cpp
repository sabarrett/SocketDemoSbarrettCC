#include "RoboCatPCH.h"

#include "GameObjects.h"

GameObjects::GameObjects(GameObjType type, GraphicsLibrary* gLib, const int networkID)
	:mObjType(type), mNetworkID(networkID)
{
	mGraphicsLib = gLib;

	mPosX = 0.0;
	mPosY = 0.0;
}

GameObjects::GameObjects(GameObjType type, GraphicsLibrary* gLib, const int networkID, float posX, float posY, const std::string imgID)
	:mObjType(type), mGraphicsLib(gLib), mNetworkID(networkID), mPosX(posX), mPosY(posY), mIMG_ID(imgID)
{

}

GameObjects::GameObjects(GameObjType type, GraphicsLibrary* gLib, const int networkID, float posX, float posY, const std::string imgID, int playerID)
	:mObjType(type), mGraphicsLib(gLib), mNetworkID(networkID), mPosX(posX), mPosY(posY), mIMG_ID(imgID), mPlayerID(playerID)
{

}

GameObjects::~GameObjects()
{
	mGraphicsLib = nullptr;
}

//---------------------Boulder-------------------/
Boulder::Boulder(GraphicsLibrary* gLib, const int networkID)
	:GameObjects(GameObjType::BOULDER, gLib, networkID)
{

}

Boulder::Boulder(GraphicsLibrary* gLib, const int networkID, const std::string imgID, float posX, float posY)
	: GameObjects(GameObjType::BOULDER, gLib, networkID, posX, posY, imgID)
{

}

void Boulder::draw()
{
	mGraphicsLib->drawScaledImage(mIMG_ID, mPosX, mPosY, 0.25, 0.25);
}

//---------------------Bubble-------------------/
Bubble::Bubble(GraphicsLibrary* gLib, const int networkID, const std::string imgID, float posX, float posY, int playerID)
	: GameObjects(GameObjType::BUBBLE, gLib, networkID, posX, posY, imgID, playerID)
{
	(playerID == 0) ? setPlayerColor(Colour(0, 0, 255)) : setPlayerColor(Colour(255, 0, 0));
}

void Bubble::draw()
{
	//mGraphicsLib->drawScaledImage(mIMG_ID, mPosX, mPosY, 0.15, 0.15);
	//mGraphicsLib->drawTintedImage(mIMG_ID, mPosX, mPosY, getPlayerColor());
	mGraphicsLib->drawTintedScaledImage(mIMG_ID, mPosX, mPosY, getPlayerColor(), 0.15, 0.15);
}

void Bubble::update()
{
	mPosY += mBubbleSpeed;
}

//---------------------Bees-------------------/
Bees::Bees(GraphicsLibrary* gLib, const int networkID, const std::string imgID, float posX, float posY)
	: GameObjects(GameObjType::BEE, gLib, networkID, posX, posY, imgID)
{

}

void Bees::draw()
{
	mGraphicsLib->drawScaledImage(mIMG_ID, mPosX, mPosY, 0.25, 0.25);
}

