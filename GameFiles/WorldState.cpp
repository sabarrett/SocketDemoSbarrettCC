#include "RoboCatPCH.h"
#include "WorldState.h"
#include "Lock.h"
#include "Key.h"
#include "Coin.h"
#include "PlayerMarcher.h"
#include "allegro_wrapper_functions-main/GraphicsLibrary.h"

unsigned int WorldState::currentInputNum = 0;

WorldState::WorldState(GraphicsLibrary* gl)
{
	mpGraphicsLibrary = gl;
	mpGameObjectLinker = new LinkingContext();
	mGameObjects = std::vector<GameObject*>();
}

WorldState::~WorldState()
{
	ClearGameObjectsOut();
	
	// bb - I commented this out because this class doesn't own this pointer
	//mpGraphicsLibrary = nullptr;
}

void WorldState::Update(bool isCreator, vector<JoinerInput>& joinerInputs, int delta)
{
	if (isCreator)
	{
		for (int i = 0; i < joinerInputs.size(); i++)
		{
			if (joinerInputs[i].inputIDType == JoinerInput::JOINER_KEY_SPAWN)
			{
				if (joinerInputs[i].inputID > WorldState::currentInputNum)
				{
					int count = joinerInputs[i].inputID - WorldState::currentInputNum;
					std::cout << "for count = " << count << ".\n";

					for (int i = 0; i < (count); i++)
					{
						std::cout << "2\n";
						CreateKey(1500, 375); // x = best guess for starting position, y  = middle of screen
						std::cout << "3\n";
					}
					std::cout << "4\n";
					WorldState::currentInputNum = joinerInputs[i].inputID;

					ChangeMarcherGoal(joinerInputs[i].location, false);

				}

			}
		}
		joinerInputs.clear();
	}

	for(int i = 0; i < mGameObjects.size(); i++)
	{
		mGameObjects[i]->Update(this, delta);
	}

	RemoveUnneededGameObjects();
}

void WorldState::Render(string background)
{
	//std::cout << 5.1 << '\n';
	mpGraphicsLibrary->drawImage(background, 0, 0);

	//std::cout << 5.2 << '\n';
	for each (GameObject * var in mGameObjects)
	{
		//std::cout << 5.3 << '\n';
		var->Render(mpGraphicsLibrary);
		//std::cout << 5.4 << '\n';

	}

	//std::cout << 5.6 << '\n';
	if (mGameObjects.size() > 1)
	{
		mpGraphicsLibrary->render();
	}
	//std::cout << 5.9 << '\n';


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

void WorldState::CreatePlayers()
{
	GameObject* createdGameObject = PlayerMarcher::CreateInstance();
	createdGameObject->SetPostion(0, 375);
	((PlayerMarcher*)createdGameObject)->SetIsServer(true);
	((PlayerMarcher*)createdGameObject)->SetGoalPos(750, 375,true);
	mpGameObjectLinker->GetNetworkId(createdGameObject, true);
	mGameObjects.push_back(createdGameObject);
	
	createdGameObject = PlayerMarcher::CreateInstance();
	createdGameObject->SetPostion(1500, 375);
	((PlayerMarcher*)createdGameObject)->SetIsServer(false);
	((PlayerMarcher*)createdGameObject)->SetGoalPos(750, 375, false);
	mpGameObjectLinker->GetNetworkId(createdGameObject, true);
	mGameObjects.push_back(createdGameObject);
}

void WorldState::ChangeMarcherGoal(Location pos, bool isServer)
{
	int marchersAffected = 0;
	for (int j = 0; j < mGameObjects.size() && marchersAffected < 2; j++)
	{
		if (mGameObjects[j]->GetClassId() == 'MRCH')
		{
			marchersAffected++;
			((PlayerMarcher*)mGameObjects[j])->SetGoalPos(pos.x, pos.y, isServer);
		}
	}
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
	stream.Write(WorldState::currentInputNum);

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
	
	int creatorsLatestInputID;
	stream.Read(creatorsLatestInputID);
	
	int result = CheckForRoundTripTime(creatorsLatestInputID);
	if (result > -1)
	{
		std::cout << "RTT from creator for input id: " << creatorsLatestInputID << " was " << result << ".\n";
	}



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
				case 'MRCH':
					tempObj = PlayerMarcher::CreateInstance();
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
	// backwards so we don't loop out
	for (int i = mToDestroy.size() - 1; i > -1; i-- )
	{
		mpGameObjectLinker->RemoveGameObject(mToDestroy[i]);
		for (int j = mGameObjects.size() - 1; j > -1; j--)
		{
			if (mGameObjects[j] == mToDestroy[i])
			{
				mGameObjects.erase(mGameObjects.begin() + j);
				break;
			}
		}

		switch (mToDestroy[i]->GetClassId())
		{
		case 'LOCK':
			delete (Lock*)mToDestroy[i];
			break;
		case 'KEYS':
			delete (Key*)mToDestroy[i];
			break;
		case 'COIN':
			delete (Coin*)mToDestroy[i];
			break;
		default:
			break;
		}
	}

	mToDestroy.clear();
}

int WorldState::CheckForRoundTripTime(int idToCheckFor)
{
	std::map<int,int>::iterator result = JoinerInput::inputTimings.find(idToCheckFor);
	if (result != JoinerInput::inputTimings.end())
	{
		int timeDif = time(0) - result->second;
		JoinerInput::inputTimings.erase(result);
		return timeDif;
	}
	return -1;
}