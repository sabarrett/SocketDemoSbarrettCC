#pragma once
#include "GameFiles/JoinerInput.h"

class GameObject;
class LinkingContext;
class GraphicsLibrary;
class JoinerInput;
class Key;

struct Location;

class WorldState
{
	const float SCREEN_X = 1000;
	const float SCREEN_Y = 500;
	const string FILE_PATH = "\\..\\..\\images\\";
	const string CREATOR_BACKGROUND = "background.jpg";

	static unsigned int currentInputNum;

	public:
		WorldState(GraphicsLibrary* gl);
		~WorldState();
		void Update(bool isCreator, vector<JoinerInput>& joinerInput, int delta);
		void Render(string background);

		void CreateLock();
		void CreateLock(int posX, int posY);
		void CreateKey(int posX, int posY);
		void CreatePlayers();

		void ChangeMarcherGoal(Location pos, bool isServer);

		template<typename T>
		void Create()
		{
			Create<T>(0, 0);
		}

		template<typename T>
		void Create(int posX, int posY)
		{
			//Yes, the registry would be safer, I'm aware
			GameObject* createdGameObject = T::CreateInstance();
			createdGameObject->SetPostion(posX, posY);
			mpGameObjectLinker->GetNetworkId(createdGameObject, true);
			mGameObjects.push_back(createdGameObject);
		}

		void SetForDestroy(GameObject* obj);

		void Write(OutputMemoryBitStream& stream) const;
		virtual void Read(InputMemoryBitStream& stream);
	protected:
	private:
		void ClearGameObjectsOut();
		void RemoveUnneededGameObjects();

		vector<GameObject*> mGameObjects;
		vector<GameObject*> mToDestroy;
		LinkingContext* mpGameObjectLinker;
		GraphicsLibrary* mpGraphicsLibrary;
		int CheckForRoundTripTime(int idToCheckFor);


	friend Key;
};

