#include "RoboCatPCH.h"
#include "GameObject.h"

GameObject::GameObject(const int gameID) : mGameID(gameID)
{
	mPosX = 0.0;
	mPosY = 0.0;
}

GameObject::GameObject(const int gameID, float posX, float posY) : mGameID(gameID)
{
	mPosX = posX;
	mPosY = posY;
}

GameObject::~GameObject()
{

}