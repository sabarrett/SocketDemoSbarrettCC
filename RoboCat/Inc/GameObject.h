#pragma once

#include <utility>
#include "GraphicsLibrary.h"
#include "TransmissionData.h"

using std::pair;

/*
	File information:
	This file contains the definition for the GameObject class.
*/

enum GameObjectType
{
	INVALID = -1,
	ROCK,
	WALL,
	PLAYER,
	ENUM_SIZE
};

class GameObject : public TransmissionData
{
	//-------------------------Private data-------------------------


	//-------------------------Protected data-------------------------

protected:

	//Identifiers
	const GameObjectType mGameObjectType = GameObjectType::INVALID;
	const int mNetworkID;

	//Position
	pair<float, float> mPosition;

	//Graphics library
	GraphicsLibrary* pGraphicsLibrary;

	//Sprite identifier
	const std::string mSPRITE_IDENTIFIER;

	//Constructor(s)
	GameObject(GameObjectType gameObjectType, const int networkID, GraphicsLibrary* graphicsLibrary);
	GameObject(GameObjectType gameObjectType, const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, const std::string spriteIdentifier = "");

	//-------------------------Public data-------------------------
public:

	//Destructor
	virtual ~GameObject();

	//Accessor(s)
	const GameObjectType getGameObjectType() { return mGameObjectType; };
	const int getNetworkID() { return mNetworkID; };
	const pair<float, float> getPosition() { return mPosition; };

	//Mutator(s)
	float setPosX(float posX) { mPosition.first = posX; };
	float setPosY(float posY) { mPosition.second = posY; };
	void setPos(pair<float, float> newPos) { mPosition = newPos; };

	//Functions
	virtual void update() = 0;
	virtual void draw() = 0;
};