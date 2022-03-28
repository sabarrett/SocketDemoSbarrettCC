#include "GameObject.h"
#include "RoboCatPCH.h"

GameObject::GameObject(const int gameObjectID, const int networkID)
	: mID(gameObjectID), mNetworkID(networkID)
{
	//Position
	mPosition.first = 0.0;
	mPosition.second = 0.0;
}

GameObject::GameObject(const int gameObjectID, const int networkID, pair<float, float> position)
	: mID(gameObjectID), mNetworkID(networkID)
{
	//Position
	mPosition = position;
}
GameObject::~GameObject()
{

}