#include "GameObject.h"
#include "RoboCatPCH.h"

GameObject::GameObject(const int gameObjectID, const int networkID)
	: mID(gameObjectID), mNetworkID(networkID)
{
	//Position
	mPosX = 0.0;
	mPosY = 0.0;
}

GameObject::GameObject(const int gameObjectID, const int networkID, float posX, float posY)
	: mID(gameObjectID), mNetworkID(networkID)
{
	//Position
	mPosX = posX;
	mPosY = posY;
}
GameObject::~GameObject()
{

}