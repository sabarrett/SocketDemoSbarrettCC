#pragma once

/*
	File information:
	This file contains the definition for the Rock class, derived from GameObject
*/

#include "GameObject.h"

class Rock : public GameObject
{
	//-------------------------Private data-------------------------


	//-------------------------Public data-------------------------
public:

	//Constructor(s)
	Rock(const int networkID, GraphicsLibrary* graphicsLibrary);
	Rock(const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, const std::string spriteIdentifier);

	//Destructor
	~Rock();

	//Accessor(s)


	//Mutator(s)


	//Functions
	void update();
	void draw();
};