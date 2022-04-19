#include "Game.h"
#include "GraphicsSystem.h"
#include "Color.h"
#include "Animation.h"
#include "AnimationData.h"
#include "Player.h"
#include "Timer.h"
#include "UnitManager.h"
#include "AnimationManager.h"
#include "GraphicsBufferManager.h"
#include "EventSystem.h"
#include "GameListener.h"
#include "../RedNet/Header Files/TCPNetworkManager.h"
#include "Unit.h"
#include "TCPNetworkManager.h"

#include <iostream>

using namespace std;

Game* Game::mspInstance = nullptr;

Game* Game::getInstance()
{
	if (!mspInstance)
		mspInstance = new Game();

	return mspInstance;
}

void Game::cleanupInstance()
{
	if (mspInstance)
	{
		delete mspInstance;
		mspInstance = nullptr;
	}
}

void Game::startGame()
{
	mIsPlaying = true;
	mpFrameTimer->start();
	mpGameTimer->start();
	while (mIsPlaying)    // Detect window close button or ESC key
	{
		deltaTime = mpFrameTimer->getElapsedTime();
		mpFrameTimer->start();

		getInput();
		update();
		render();

		debug();

		while(mpFrameTimer->getElapsedTime() < mTimePerFrame)
			mpFrameTimer->sleepUntilElapsed(mTimePerFrame);
			
	}
}

Game::Game()
{
	mpGraphicsSystem = nullptr;
	mpPlayerUnit = nullptr;
	mpFrameTimer = nullptr;
	mpGameTimer = nullptr;

	mpUnitManager = nullptr;
	mpGraphicsBufferManager = nullptr;
	mpAnimationManager = nullptr;

	mpGameListener = nullptr;

	mDebugMode = false;
	mIsPlaying = false;
	mTimePerFrame = 0.0f;

}

Game::~Game()
{
	
}

void Game::init(int screenWidth, int screenHeight, int fps, bool isServer, bool debugMode)
{
	SetExitKey(-1);
	SetTargetFPS(999);

	mpGraphicsSystem = new GraphicsSystem();

	mpGraphicsSystem->init(screenWidth, screenHeight);

	mpUnitManager = new UnitManager();

	mpGraphicsBufferManager = new GraphicsBufferManager();
	GraphicsBuffer* smurfBuffer = mpGraphicsBufferManager->createAndManageGraphicsBuffer("smurf", GAMEASSETS + SMURF_FILENAME);
	GraphicsBuffer* projBuffer = mpGraphicsBufferManager->createAndManageGraphicsBuffer("proj", GAMEASSETS + PROJECTILE_FILENAME);
	mpGraphicsBufferManager->createAndManageGraphicsBuffer("background", GAMEASSETS + BACKGROUND_FILEPATH);

	mpAnimationManager = new AnimationManager();

	AnimationData* playerAnimData = mpAnimationManager->createAndManageAnimationData("smurf", smurfBuffer, 4, 4);
	mpAnimationManager->createAndManageAnimationData("proj", projBuffer, 1, 13, 0.25f);

	Animation* playerAnim = mpAnimationManager->createAndManageAnimation(playerAnimData, 16);

	if (isServer)
	{
		mpPlayerUnit = new Player(playerAnim, 200.0f, Vector2D(300, 300));
		mpOpponent = mpUnitManager->createAndManageUnit(playerAnim, -1, Vector2D(700, 200)); //-1 for an ID represents a non-replicated unit
	}
	else
	{
		mpPlayerUnit = new Player(playerAnim, 200.0f, Vector2D(700, 200));
		mpOpponent = mpUnitManager->createAndManageUnit(playerAnim, -1, Vector2D(300, 300));
	}

	mpGameListener = new GameListener();
	EventSystem::getInstance()->addListener(PLAYER_MOVE_EVENT, mpGameListener);
	EventSystem::getInstance()->addListener(KEYBOARD_EVENT, mpGameListener);
	EventSystem::getInstance()->addListener(MOUSE_ACTION_EVENT, mpGameListener);

	mpFrameTimer = new Timer();
	mpGameTimer = new Timer();

	mTimePerFrame = 1.0f / fps;
	mDebugMode = debugMode;
	mIsServer = isServer;

	if (isServer)
	{
		TCPNetworkManager::getInstance()->init("127.0.0.1", 8011, 70);
		TCPNetworkManager::getInstance()->listenAndAccept();
	}
	else
	{
		TCPNetworkManager::getInstance()->init("127.0.0.1", 8012, 70);
		TCPNetworkManager::getInstance()->connectTo("127.0.0.1", 8011);
	}
		
	playerPacketTime = 0;

	srand(time(NULL));
}

void Game::cleanup()
{
	EventSystem::getInstance()->removeListenerFromAllEvents(mpGameListener);
	delete mpGameListener;
	mpGameListener = nullptr;

	delete mpPlayerUnit;
	mpPlayerUnit = nullptr;

	delete mpUnitManager;
	mpUnitManager = nullptr;

	delete mpAnimationManager;
	mpAnimationManager = nullptr;

	delete mpGraphicsBufferManager;
	mpGraphicsBufferManager = nullptr;

	EventSystem::cleanupInstance();

	InputSystem::cleanupInstance();

	mpGraphicsSystem->cleanup();

	delete mpGraphicsSystem;
	mpGraphicsSystem = nullptr;

	delete mpFrameTimer;
	mpFrameTimer = nullptr;

	delete mpGameTimer;
	mpGameTimer = nullptr;
}

void Game::getInput()
{
	InputSystem::getInstance()->inputUpdate(mpGameTimer->getElapsedTime());
}

void Game::update()
{
	if (mIsServer)
	{
		mpUnitManager->update(deltaTime);

		for (int i = mpUnitManager->getNumOfUnits()-1; i > 0; i--)
		{
			int id = mpUnitManager->getIDAt(i);
			//cout << id << " ";
			if (id < 0)
				continue;

			Unit* u = mpUnitManager->getUnitAt(i);
			Vector2D pos = u->getLocation();

			int dataLength = sizeof(id) + sizeof(pos);
			char* data = new char[dataLength]();

			memcpy(data, (char*)&id, sizeof(id));
			memcpy(data + sizeof(id), (char*)&pos, sizeof(pos));

			TCPNetworkManager::getInstance()->sendPacket(Packet_Header::OBJECT_MOVE, data, dataLength, mpGameTimer->getElapsedTime());

			delete[] data;
		}
	}

	mpAnimationManager->update(deltaTime);

	mpPlayerUnit->setMoveDirection(InputSystem::getInstance()->getMovementAxis().normalized());
	mpPlayerUnit->update(deltaTime, mpGameTimer->getElapsedTime());

	TCPNetworkManager::getInstance()->update(deltaTime);
	TCPNetworkManager::getInstance()->receivePackets(handleNetworkPacket);
}

void Game::render()
{
	RColor lightGrey = RColor(150, 150, 150, 255);
	RColor white = RColor(255, 255, 255, 255);

	mpGraphicsSystem->clearScreenToColor(white);

	mpGraphicsSystem->draw(mpGraphicsBufferManager->getGraphicsBuffer("background"), Vector2D::Zero());

	mpPlayerUnit->draw(mpGraphicsSystem);

	mpUnitManager->draw(mpGraphicsSystem);

	mpGraphicsSystem->flip();
}

void Game::debug()
{
	if(mDebugMode)
	{
		//cout << "Frame Length: " << deltaTime << ", which is equal to " << 1 / deltaTime << " FPS." << endl;
	}
}

void Game::DPlayerMove(Vector2D loc)
{
	if(mDebugMode)
		cout << "Player move to: " << loc << endl;
}

void Game::DKeyPress(KeyCode key)
{
	if(mDebugMode)
		cout << "Key pressed with ID: " << key << endl;
}

void Game::DMousePress(int button)
{
	if (mDebugMode)
		cout << "Mouse Button pressed with ID: " << button << endl;
}

void Game::DKeyRelease(KeyCode key)
{
	if (mDebugMode)
		cout << "Key released with ID: " << key << endl;
}

void Game::DMouseRelease(int button)
{
	if (mDebugMode)
		cout << "Mouse Button released with ID: " << button << endl;
}

void Game::quitGame()
{
	mIsPlaying = false;
	cout << "QUIT" << endl;
}

void Game::handleNetworkPacket(Packet_Header header, char* data, int length)
{
	Game* gameInstance = getInstance();

	//cout << "I gotta packet boys!" << endl;
	int id;
	Vector2D pos;
	Unit* u;
	float t;

	switch (header)
	{
	case PLAYER_MOVE:
		//cout << "WE MOVING!!" << endl;
		memcpy((char*)&t, data, sizeof(t));
		memcpy((char*)&pos, data + sizeof(t), sizeof(pos));

		if (t > gameInstance->playerPacketTime)
		{
			gameInstance->mpOpponent->setLocation(pos);
			gameInstance->playerPacketTime = t;
		} 
		else //Otherwise, drop the stale packet
		{
			cout << "Dropped a stale PLAYER_MOVE packet:" << t << " < " << gameInstance->playerPacketTime << endl;
		}
	
		break;

	case FIRE_PROJECTILE:
		//cout << "FIRE IN THE HOLE!" << endl;

		memcpy((char*)&t, data, sizeof(t));
		memcpy((char*)&id, data, sizeof(id));
		memcpy((char*)&pos, data + sizeof(id), sizeof(pos));

		if (t > gameInstance->playerPacketTime)
		{
			gameInstance->fireOppProj(id, pos);
			gameInstance->playerPacketTime = t;
		}
		else //Otherwise, drop the stale packet
		{
			cout << "Dropped a stale FIRE_PROJECTILE packet:" << t << " < " << gameInstance->playerPacketTime << endl;
		}

		break;

	case OBJECT_MOVE:

		memcpy((char*)&t, data, sizeof(t));
		memcpy((char*)&id, data + sizeof(t), sizeof(id));
		memcpy((char*)&pos, data + sizeof(t) + sizeof(id),  sizeof(pos));

		if (t > gameInstance->playerPacketTime)
		{
			u = gameInstance->mpUnitManager->getUnitAtID(id);

			if (u)
			{
				u->setLocation(pos);
			}

			gameInstance->playerPacketTime = t;
		}
		else //Otherwise, drop the stale packet
		{
			cout << "Dropped a stale OBJECT_MOVE packet:" << t << " < " << gameInstance->playerPacketTime << endl;
		}

		//cout << "MOVE OBJECT: " << id << endl;
		break;

	case OBJECT_DELETE:
		memcpy((char*)&id, data, sizeof(id));

		gameInstance->mpUnitManager->removeAndDeleteUnit(gameInstance->mpUnitManager->getUnitAtID(id));
		break;

	case CAMERA_MOVE:
		memcpy((char*)&pos, data, sizeof(pos));

		gameInstance->mpGraphicsSystem->setCameraLocation(pos);
	}

}

int Game::fireProj()
{
	Vector2D dir = Vector2D(
		InputSystem::getInstance()->getMousePosition().getX() - mpPlayerUnit->getLocation().getX() + mpGraphicsSystem->getCameraLocation().getX(), 
		InputSystem::getInstance()->getMousePosition().getY() - mpPlayerUnit->getLocation().getY() + mpGraphicsSystem->getCameraLocation().getY());
	dir.normalize();

	AnimationData* projAnimData = mpAnimationManager->getAnimationData("proj");
	Animation* projAnim = mpAnimationManager->createAndManageAnimation(projAnimData, 13);

	Unit* u = mpUnitManager->createAndManageUnit(projAnim, 0, mpPlayerUnit->getLocation(), dir, PROJECTILE_SPEED);

	return mpUnitManager->getIDAt(mpUnitManager->find(u));
}

void Game::fireOppProj(int id, Vector2D loc)
{
	Vector2D dir = Vector2D(
		loc.getX() - mpOpponent->getLocation().getX() + mpGraphicsSystem->getCameraLocation().getX(),
		loc.getY() - mpOpponent->getLocation().getY() + mpGraphicsSystem->getCameraLocation().getY());
	dir.normalize();

	AnimationData* projAnimData = mpAnimationManager->getAnimationData("proj");
	Animation* projAnim = mpAnimationManager->createAndManageAnimation(projAnimData, 13);

	mpUnitManager->createAndManageUnit(projAnim, id, mpOpponent->getLocation(), dir, PROJECTILE_SPEED);
}