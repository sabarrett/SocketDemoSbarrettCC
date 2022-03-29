#include "RoboCatPCH.h"
#include "WorldState.h"
#include "Lock.h"
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

void WorldState::Update()
{
	for each (GameObject* var in mGameObjects)
	{
		var->Update();
	}
}

void WorldState::Render()
{
	//mpGraphicsLibrary->drawImage(BACKGROUND, 0, 0);

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
