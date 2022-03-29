
#include "RoboCatPCH.h"
#include "JoinerInput.h"

JoinerInput::JoinerInput(InputActionTypes initalType, Location loc)
{
	timeOfCreation = time(0);
	initalType = type;
	location = loc;
}

void JoinerInput::Write(OutputMemoryBitStream& stream)
{
	stream.Write(timeOfCreation);
	stream.Write(type);
	stream.Write(location.x);
	stream.Write(location.y);
}

void JoinerInput::Read(InputMemoryBitStream& stream)
{
	stream.Read(timeOfCreation);
	stream.Read(type);
	stream.Read(location.x);
	stream.Read(location.y);
}