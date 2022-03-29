#include "RoboCatPCH.h"
#include "WorldState.h"
#include "Lock.h"
#include "Key.h"
#include "Coin.h"
#include "allegro_wrapper_functions-main/GraphicsLibrary.h"

WorldState::WorldState(GraphicsLibrary* gl)
{
	mpGraphicsLibrary = gl;
	mpGameObjectLinker = new LinkingContext();
	mGameObjects = std::vector<GameObject*>();
}

WorldState::~WorldState()
{
	delete mpGameObjectLinker;
	mpGraphicsLibrary = nullptr;
}

void WorldState::Update(bool isCreator, vector<JoinerInput>& joinerInput)
{
	if (isCreator)
	{
		// 
	}

	for each (GameObject* var in mGameObjects)
	{
		var->Update();
	}
}

void WorldState::Render()
{
	mpGraphicsLibrary->drawImage(BACKGROUND, 0, 0);

	for each (GameObject * var in mGameObjects)
	{
		var->Render(mpGraphicsLibrary);
	}

	mpGraphicsLibrary->render();
}

void WorldState::CreateLock()
{
	CreateLock(0, 0);
}

void WorldState::CreateLock(int posX, int posY)
{
	GameObject* createdGameObject = Lock::CreateInstance();
	createdGameObject->SetPostion(posX, posY);
	mpGameObjectLinker->GetNetworkId(createdGameObject,true);
	mGameObjects.push_back(createdGameObject);
}

void WorldState::Write(OutputMemoryBitStream& stream) const
{
	int count = mGameObjects.size();
	GameObject* tempObj;
	stream.Write(count);
	for (int i = 0; i < count; i++)
	{
		tempObj = mGameObjects[i];
		stream.Write(mpGameObjectLinker->GetNetworkId(tempObj,true));
		stream.Write(tempObj->GetClassId());
		tempObj->Write(stream);
	}
}

void WorldState::Read(InputMemoryBitStream& stream)
{
	int count;
	uint32_t networkID;
	uint32_t classID;
	GameObject* tempObj;
	stream.Read(count);
	for (int i = 0; i < count; i++)
	{
		stream.Read(networkID);
		stream.Read(classID);
		
		tempObj = mpGameObjectLinker->GetGameObject(networkID);

		if (tempObj == nullptr)
		{
			switch (classID)
			{
				case 'LOCK':
					tempObj = Lock::CreateInstance();
					break;
				case 'KEYS':
					tempObj = Key::CreateInstance();
					break;
				case 'COIN':
					tempObj = Coin::CreateInstance();
					break;
				default:
					break;
			}
			mpGameObjectLinker->AddGameObject(tempObj, networkID);
			mGameObjects.push_back(tempObj);
		}
		
		tempObj->Read(stream);
	}
}
