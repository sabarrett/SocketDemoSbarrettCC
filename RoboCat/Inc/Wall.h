#pragma once

/*
	File information:
	This file contains the definition for the Wall class, derived from GameObject
*/

#include "GameObject.h"
#include "Colour.h"

class Wall : public GameObject
{
	//-------------------------Private data-------------------------
	float mSizeX;
	float mSizeY;
	float mThickness;

	Colour mColour;

	//-------------------------Public data-------------------------
public:

	//Constructor(s)
	Wall(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary);
	Wall(const int gameObjectID, const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, float sizeX, float sizeY, Colour colour, float thickness);

	//Destructor
	~Wall();

	//Accessor(s)
	float getWallSizeX() { return mSizeX; };
	float getWallSizeY() { return mSizeY; };

	//Mutator(s)
	void setWallSizeX(float sizeX) { mSizeX = sizeX; };
	void setWallSizeY(float sizeY) { mSizeY = sizeY; };

	//Functions
	void update();
	void draw();
	void read(InputMemoryBitStream& stream);
	void write(OutputMemoryBitStream& stream) const;
};