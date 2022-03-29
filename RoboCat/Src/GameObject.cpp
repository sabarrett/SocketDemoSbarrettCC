#include "GameObject.h"
#include "RoboCatPCH.h"

GameObject::GameObject(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary)
	: mID(gameObjectID), mNetworkID(networkID)
{
	//Graphics library
	pGraphicsLibrary = graphicsLibrary;

	//Position
	mPosition.first = 0.0;
	mPosition.second = 0.0;
}

GameObject::GameObject(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position)
	: mID(gameObjectID), mNetworkID(networkID)
{
	//Graphics library
	pGraphicsLibrary = graphicsLibrary;

	//Position
	mPosition = position;
}
GameObject::~GameObject()
{
	//Graphics library gets cleaned up in main.cpp, just de-reference it here
	pGraphicsLibrary = nullptr;
}