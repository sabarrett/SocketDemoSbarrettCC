#include "RoboCatPCH.h"
#include "GameObject.h"

GameObject::GameObject(const int gameID, const int netID) : mGameID(gameID), mNetID(netID)
{
	mPosX = 0.0;
	mPosY = 0.0;
}

GameObject::GameObject(const int gameID, const int netID, float posX, float posY) : mGameID(gameID), mNetID(netID)
{
	mPosX = posX;
	mPosY = posY;
}

GameObject::~GameObject()
{

}