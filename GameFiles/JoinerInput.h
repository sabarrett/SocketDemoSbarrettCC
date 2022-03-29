#pragma once
#include <stdlib.h>
#include "GameObject.h"

class JoinerInput 
{
	public:

	enum InputActionTypes
	{
		JOINER_KEY_SPAWN
	};

	time_t timeOfCreation;
	InputActionTypes type;
	Location location;

	JoinerInput(InputActionTypes initalType, Location loc);

	void Write(OutputMemoryBitStream& stream);
	void Read(InputMemoryBitStream& stream);
};