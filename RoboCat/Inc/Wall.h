#pragma once

/*
	File information:
	This file contains the definition for the Wall class, derived from GameObject
*/

#include "GameObject.h"

class Wall : public GameObject
{
	//-------------------------Private data-------------------------
	float mSizeX;
	float mSizeY;


	//-------------------------Public data-------------------------
public:

	//Constructor(s)
	Wall(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary);
	Wall(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, float sizeX, float sizeY, const std::string spriteIdentifier);

	//Destructor
	~Wall();

	//Accessor(s)
	float getWallSizeX() { return mSizeX; };
	float getWallSizeY() { return mSizeY; };

	//Mutator(s)
	float setWallSizeX(float sizeX) { mSizeX = sizeX; };
	float setWallSizeY(float sizeY) { mSizeY = sizeY; };

	//Functions
	void update();
	void draw();
};