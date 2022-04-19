
//#include "RoboCatPCH.h"
#include "InputSystem.h"
#include "GraphicsLibrary.h"
#include "Colour.h"
#include "GameObject.h"
#include "NetworkManager.h"
#include "NetworkManagerServer.h"

const int MAX_OBJECT_COUNT = 100; //i think this might be needed i am unsure
std::string BACKGROUND_PATH = "..\\..\\Assets\\Woods.png"; //I think

#if _WIN32

int main(int argc, const char** argv)
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#else
const char** __argv;
int __argc;
int main(int argc, const char** argv)
{
	__argc = argc;
	__argv = argv;
#endif

	SocketUtil::StaticInit();

	al_init();

	GameObject mGameObjects[MAX_OBJECT_COUNT]; //possibly make this an array with MAX_OBJECT_COUNT
	GraphicsLibrary* mpGraphicsLibrary = new GraphicsLibrary(1600, 1200);
	InputSystem* mpInputSystem = new InputSystem();
	NetworkManager mNetworkManager;
	NetworkManagerServer mNetworkManagerServer;

	bool activeConnection = true;
	bool isServer;
	int unitCount = 0; //use this to track changes for deleting/making units
	int pastUnitCount = 0;

	mpGraphicsLibrary->init(BACKGROUND_PATH);
	mpInputSystem->init(mpGraphicsLibrary);

	mpGraphicsLibrary->loadImage(BACKGROUND_PATH, "Woods.png");

	isServer = StringUtils::GetCommandLineArg(1) == "server";

	if (isServer)
	{
		SocketAddress servAddress(INADDR_LOOPBACK, 8081); //8081?

		mNetworkManagerServer.init(servAddress, "server");
	}
	else
	{
		SocketAddress servAddress(INADDR_LOOPBACK, 8080);

		mNetworkManager.init(servAddress, "client");
	}

	mpGraphicsLibrary->init(BACKGROUND_PATH);

	while (activeConnection == true)
	{
		pastUnitCount = unitCount;

		if (mpInputSystem->getKeyboardInput() == KeyCode::S)
		{
			//spawn an object
			mGameObjects[unitCount].CreateObject();

			unitCount++;
		}

		if (mpInputSystem->getKeyboardInput() == KeyCode::D)
		{
			//despawn an object
			int tmpRand;
			GameObject tmpObjects[MAX_OBJECT_COUNT];

			tmpRand = rand() % MAX_OBJECT_COUNT;

			mGameObjects[tmpRand].DeleteObject(); //could be unneeded

			for (int i = 0; i < unitCount; i++)
			{
				if (i != tmpRand)
				{
					tmpObjects[i] = mGameObjects[i];
				}
			}

			unitCount--;

			for (int i = 0; i < unitCount; i++)
			{
				mGameObjects[i] = tmpObjects[i];
			}
		}

		if (mpInputSystem->getKeyboardInput() == KeyCode::Esc)
		{
			//basically exit the game and shut everything down
			activeConnection = false;
		}

		for (int i = 0; i < unitCount; i++)
		{
			mGameObjects[i].UpdatePosition();
			mGameObjects[i].DrawObjects();
		}

		//now i need to move and send this info
		if (isServer)
		{
			mNetworkManagerServer.update(mGameObjects);
		}
		else
		{
			mNetworkManager.update(mGameObjects);
		}

		//mpGraphicsLibrary->drawImage("Woods.png", 0, 0); //i need to make this work
		mpGraphicsLibrary->render(); //render at the very end
	}
	
	delete mpGraphicsLibrary;
	mpGraphicsLibrary = NULL; //is this needed?
	delete mpInputSystem;
	mpInputSystem = NULL;

	//delete mNetworkManagerServer
	//delete mNetworkManager

	SocketUtil::CleanUp();

	return 0;
}