#pragma once

/*
	File information:
	This file contains the definition for the GameObject class.
*/

class GameObject
{
	//-------------------------Private data-------------------------

	//Identifiers
	int mID;
	int mNetworkID;

	//Position
	float mPosX;
	float mPosY;

	//-------------------------Public data-------------------------
public:

	//Constructor(s)
	GameObject(int gameObjectID, int networkID);

	//Destructor
	~GameObject();

	//Accessor(s)
	int getID() { return mID; };
	float getPosX() { return mPosX; };
	float getPosY() { return mPosY; };

	//Mutator(s)
	float setPosX(float posX) { mPosX = posX; };
	float setPosY(float posY) { mPosY = posY; };

	//Functions
	
};