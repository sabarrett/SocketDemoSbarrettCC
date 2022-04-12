
#include "RoboCatPCH.h"
#include "JoinerInput.h"

JoinerInput::JoinerInput(InputActionIDs initalType, Location loc)
{
	timeOfCreation = time(0);
	inputIDType = initalType;
	location = loc;
}

void JoinerInput::Write(OutputMemoryBitStream& stream)
{
	stream.Write(inputIDType);
	stream.Write(timeOfCreation);
	stream.Write(location.x);
	stream.Write(location.y);
}

void JoinerInput::Write(OutputMemoryBitStream& stream, vector<JoinerInput>& joinerInputs)
{
	GameObject* tempObj;
	stream.Write(joinerInputs.size());
	for (int i = 0; i < joinerInputs.size(); i++)
	{
		joinerInputs[i].Write(stream);
	}
}

void JoinerInput::Read(InputMemoryBitStream& stream, vector<JoinerInput>& joinerInputs)
{
	int count; 
	stream.Read(count);
	for (int i = 0; i < count; i++)
	{
		JoinerInput current;
		stream.Read(current.inputIDType);
		stream.Read(current.timeOfCreation);
		stream.Read(current.location.x);
		stream.Read(current.location.y);

		joinerInputs.push_back(current);
	}
}