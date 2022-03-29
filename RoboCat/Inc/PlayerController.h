#pragma once

/*
	File information:
	This file contains the definition for the PlayerController class, derived from GameObject
*/

#include "GameObject.h"
#include "InputSystem.h"

class PlayerController : public GameObject
{
	//-------------------------Private data-------------------------

	//Reference to input system
	InputSystem* pInput;

	//Movement data
	float mMoveSpeed;

	//-------------------------Public data-------------------------
public:

	//Constructor(s)
	PlayerController(const int gameObjectID, const int networkID, InputSystem* inputSystem);
	PlayerController(const int gameObjectID, const int networkID, pair<float, float> position, float moveSpeed, InputSystem* inputSystem);

	//Destructor
	~PlayerController();

	//Accessor(s)
	float getMoveSpeed() { return mMoveSpeed; };

	//Mutator(s)
	void setMoveSpeed(float moveSpeed) { mMoveSpeed = moveSpeed; };

	//Functions
	void update();
	void draw();
};