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
	ClearGameObjectsOut();
	mpGraphicsLibrary = nullptr;
}

void WorldState::Update(bool isCreator, vector<JoinerInput>& joinerInputs)
{
	if (isCreator)
	{
		for (int i = 0; i < joinerInputs.size(); i++)
		{
			CreateKey(joinerInputs[i].location.x, joinerInputs[i].location.y);
		}
		joinerInputs.clear();
	}

	for(int i = 0; i < mGameObjects.size(); i++)
	{
		mGameObjects[i]->Update(this);
	}

	RemoveUnneededGameObjects();
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
	createdGameObject->SetPostion(posX, 375); // quick fix so it's in middle
	mpGameObjectLinker->GetNetworkId(createdGameObject,true);
	mGameObjects.push_back(createdGameObject);
}

void WorldState::CreateKey(int posX, int posY)
{
	GameObject* createdGameObject = Key::CreateInstance();
	createdGameObject->SetPostion(posX, 375); // quick fix so it's in the middle
	mpGameObjectLinker->GetNetworkId(createdGameObject,true);
	mGameObjects.push_back(createdGameObject);
}

void WorldState::SetForDestroy(GameObject* obj)
{
	mToDestroy.push_back(obj);
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
	ClearGameObjectsOut();
	mpGameObjectLinker = new LinkingContext();

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

void WorldState::ClearGameObjectsOut()
{
	while (!mGameObjects.empty())
	{
		GameObject* obj = mGameObjects.back();
		switch (obj->GetClassId())
		{
		case 'LOCK':
			delete (Lock*)obj;
			break;
		case 'KEYS':
			delete (Key*)obj;
			break;
		case 'COIN':
			delete (Coin*)obj;
			break;
		default:
			break;
		}
		mGameObjects.pop_back();
	}

	delete mpGameObjectLinker;
}

void WorldState::RemoveUnneededGameObjects()
{
	for each(GameObject* obj in mToDestroy)
	{
		mpGameObjectLinker->RemoveGameObject(obj);
		for (int i = 0; i < mGameObjects.size(); i++)
		{
			if (mGameObjects[i] == obj)
			{
				mGameObjects.erase(mGameObjects.begin() + i);
				break;
			}
		}
		
		switch (obj->GetClassId())
		{
		case 'LOCK':
			delete (Lock*)obj;
			break;
		case 'KEYS':
			delete (Key*)obj;
			break;
		case 'COIN':
			delete (Coin*)obj;
			break;
		default:
			break;
		}
	}

	mToDestroy.clear();
}
