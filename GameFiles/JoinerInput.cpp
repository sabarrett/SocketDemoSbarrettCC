
#include "RoboCatPCH.h"
#include "JoinerInput.h"

JoinerInput::JoinerInput(InputActionTypes initalType, Location loc)
{
	timeOfCreation = time(0);
	initalType = type;
	location = loc;
}

