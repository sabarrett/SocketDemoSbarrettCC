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

GameObject::GameObject(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, const std::string spriteIdentifier)
	: mID(gameObjectID), mNetworkID(networkID), mSPRITE_IDENTIFIER(spriteIdentifier)
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