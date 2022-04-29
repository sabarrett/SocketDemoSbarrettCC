#pragma once
#include <stdlib.h>
#include "GameObject.h"
#include "GameFiles/WorldState.h"


class JoinerInput 
{
	public:
		

	enum InputActionIDs
	{
		JOINER_KEY_SPAWN,
		CONNECTION_CONFIRMATION_MESSAGE
	};

	time_t timeOfCreation;
	InputActionIDs inputIDType;
	Location location;

	int inputID;
	static unsigned int inputIdIterator;

	JoinerInput(InputActionIDs initalType, Location loc);

	static void Write(OutputMemoryBitStream& stream, vector<JoinerInput>& joinerInputs);

	static void Read(InputMemoryBitStream& stream, vector<JoinerInput>& joinerInputs);

	private:
	
		
	void Write(OutputMemoryBitStream& stream);
	JoinerInput()
	{
		;
	}
};