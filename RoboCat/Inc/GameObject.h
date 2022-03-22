#pragma once

/*
	File information:
	This file contains the definition for the GameObject class.
*/

class GameObject
{
	//-------------------------Private data-------------------------

	//Identifiers
	const int mID;
	const int mNetworkID;

	//Position
	float mPosX;
	float mPosY;

	//-------------------------Public data-------------------------
public:

	//Constructor(s)
	GameObject(const int gameObjectID, const int networkID);
	GameObject(const int gameObjectID, const int networkID, float posX, float posY);

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
	
};