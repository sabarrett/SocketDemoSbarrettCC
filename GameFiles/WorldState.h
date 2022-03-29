#pragma once
class GameObject;
class LinkingContext;
class GraphicsLibrary;
struct Location;
//class InputSystem;

class WorldState
{
	const float SCREEN_X = 1000;
	const float SCREEN_Y = 500;
	const string FILE_PATH = "\\..\\..\\images\\";
	const string BACKGROUND = "background.jpg";

	public:
		WorldState(GraphicsLibrary* gl);
		~WorldState();
		void Update();
		void Render();

		void CreateLock();
		void CreateLock(int posX, int posY);

		void CreateKey();
		void CreateKey(Location loc);

	protected:
	private:
		vector<GameObject*> mGameObjects;
		LinkingContext* mpGameObjectLinker;
		GraphicsLibrary* mpGraphicsLibrary;
		//InputSystem mpInputSystem;
};

