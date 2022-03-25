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
	Rock(const int gameObjectID, const int networkID);
	Rock(const int gameObjectID, const int networkID, float posX, float posY);

	//Destructor
	~Rock();

	//Accessor(s)


	//Mutator(s)


	//Functions
	void update();
	void draw();
};