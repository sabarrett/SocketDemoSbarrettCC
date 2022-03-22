#include "GameObject.h"
#include "RoboCatPCH.h"

GameObject::GameObject(int gameObjectID, int networkID)
{
	//Identifiers
	mID = gameObjectID;
	mNetworkID = networkID;

	//Position
	mPosX = 0.0;
	mPosY = 0.0;
}

GameObject::~GameObject()
{

}