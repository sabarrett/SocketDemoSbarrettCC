#pragma once

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
	float mPosX;
	float mPosY;

	//Constructor(s)
	GameObject(const int gameObjectID, const int networkID);
	GameObject(const int gameObjectID, const int networkID, float posX, float posY);

	//-------------------------Public data-------------------------
public:

	//Destructor
	~GameObject();

	//Accessor(s)
	const int getGameObjectID() { return mID; };
	const int getNetworkID() { return mID; };
	float getPosX() { return mPosX; };
	float getPosY() { return mPosY; };

	//Mutator(s)
	float setPosX(float posX) { mPosX = posX; };
	float setPosY(float posY) { mPosY = posY; };

	//Functions
	virtual void update() = 0;
	virtual void draw() = 0;
};