#include "GameObject.h"
#include "RoboCatPCH.h"

GameObject::GameObject(GameObjectType gameObjectType, const int networkID, GraphicsLibrary* graphicsLibrary)
	: mGameObjectType(gameObjectType), mNetworkID(networkID)
{
	//Graphics library
	pGraphicsLibrary = graphicsLibrary;

	//Position
	mPosition.first = 0.0;
	mPosition.second = 0.0;
}

GameObject::GameObject(GameObjectType gameObjectType, const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, const std::string spriteIdentifier)
	: mGameObjectType(gameObjectType), mNetworkID(networkID), mSPRITE_IDENTIFIER(spriteIdentifier)
{
	//Graphics library
	pGraphicsLibrary = graphicsLibrary;

	//Position
	mPosition = position;
}

GameObject::GameObject(const GameObject& copy)
	: mGameObjectType(copy.mGameObjectType), mNetworkID(copy.mNetworkID), mSPRITE_IDENTIFIER(copy.mSPRITE_IDENTIFIER)
{
	//Graphics library
	pGraphicsLibrary = copy.pGraphicsLibrary;

	//Position
	mPosition = copy.mPosition;
}

GameObject::~GameObject()
{
	//Graphics library gets cleaned up in main.cpp, just de-reference it here
	pGraphicsLibrary = nullptr;
}