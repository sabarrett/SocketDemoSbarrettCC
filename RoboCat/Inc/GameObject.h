#pragma once
#include <utility>

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
	ENUM_SIZE
};

class GameObject
{
	////-------------------------Private data-------------------------

	////Identifiers
	//const int mID;
	//const int mNetworkID;

	//-------------------------Protected data-------------------------

protected:

	//Identifiers
	const int mID;
	const int mNetworkID;

	//Position
	pair<float, float> mPosition;

	//Constructor(s)
	GameObject(const int gameObjectID, const int networkID);
	GameObject(const int gameObjectID, const int networkID, pair<float, float> position);

	//-------------------------Public data-------------------------
public:

	//Destructor
	virtual ~GameObject();

	//Accessor(s)
	const int getGameObjectID() { return mID; };
	const int getNetworkID() { return mID; };
	pair<float, float> getPosition() { return mPosition; };

	//Mutator(s)
	float setPosX(float posX) { mPosition.first = posX; };
	float setPosY(float posY) { mPosition.second = posY; };

	//Functions
	virtual void update() = 0;
	virtual void draw() = 0;
};