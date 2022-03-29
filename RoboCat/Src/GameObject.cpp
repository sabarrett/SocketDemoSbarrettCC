#include "RoboCatPCH.h"
#include "GameObject.h"

GameObject::GameObject(GameObjectType gameObjType, const int gameID) : mIngameID{ gameID }, mGameObjType{ gameObjType }
{
	mPosX = 0.0;
	mPosY = 0.0;
}

GameObject::GameObject(GameObjectType gameObjType, const int gameID, float posX, float posY) : mIngameID{ gameID }, mGameObjType{ gameObjType }
{
	mPosX = posX;
	mPosY = posY;
}

GameObject::GameObject(GameObjectType gameObjType, const int gameID, float posX, float posY, string imageIdentifier) : mIngameID{ gameID }, mGameObjType{ gameObjType }
{
	mPosX = posX;
	mPosY = posY;

	mImageIdentifier = imageIdentifier;
}

GameObject::~GameObject()
{

}