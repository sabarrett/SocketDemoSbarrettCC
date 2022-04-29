
#include "RoboCatPCH.h"
#include "JoinerInput.h"

unsigned int JoinerInput::inputIdIterator = 0;
std::map<int, int> JoinerInput::inputTimings = std::map<int, int>(); // <input id, time created>


JoinerInput::JoinerInput(InputActionIDs initalType, Location loc)
{
	timeOfCreation = time(0);
	inputIDType = initalType;
	location = loc;
	
	if (inputIDType == InputActionIDs::JOINER_KEY_SPAWN)
	{
		inputID = ++JoinerInput::inputIdIterator;
		inputTimings.emplace(inputID, time(0));
	}
	else
		inputID = 00; //these don't need an id

}

void JoinerInput::Write(OutputMemoryBitStream& stream)
{
	stream.Write(inputIDType);
	stream.Write(timeOfCreation);
	stream.Write(location.x);
	stream.Write(location.y);
	stream.Write(inputID);
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
		stream.Read(current.inputID);

		joinerInputs.push_back(current);
	}
}