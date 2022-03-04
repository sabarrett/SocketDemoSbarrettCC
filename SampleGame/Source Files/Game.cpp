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
	mpGameTimer->start();
	while (mIsPlaying)    // Detect window close button or ESC key
	{
		deltaTime = mpGameTimer->getElapsedTime();
		mpGameTimer->start();

		getInput();
		update();
		render();

		debug();

		while(mpGameTimer->getElapsedTime() < mTimePerFrame)
			mpGameTimer->sleepUntilElapsed(mTimePerFrame);
			
	}
}

Game::Game()
{
	mpGraphicsSystem = nullptr;
	mpPlayerUnit = nullptr;
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

void Game::init(int screenWidth, int screenHeight, int fps, bool debugMode)
{
	SetExitKey(-1);
	SetTargetFPS(999);

	mpGraphicsSystem = new GraphicsSystem();

	mpGraphicsSystem->init(screenWidth, screenHeight);

	mpUnitManager = new UnitManager();

	mpGraphicsBufferManager = new GraphicsBufferManager();
	GraphicsBuffer* smurfBuffer = mpGraphicsBufferManager->createAndManageGraphicsBuffer("smurf", ASSET_PATH + SMURF_FILENAME);
	GraphicsBuffer* projBuffer = mpGraphicsBufferManager->createAndManageGraphicsBuffer("proj", ASSET_PATH + PROJECTILE_FILENAME);
	mpGraphicsBufferManager->createAndManageGraphicsBuffer("background", ASSET_PATH + BACKGROUND_FILEPATH);

	mpAnimationManager = new AnimationManager();

	AnimationData* playerAnimData = mpAnimationManager->createAndManageAnimationData("smurf", smurfBuffer, 4, 4);
	mpAnimationManager->createAndManageAnimationData("proj", projBuffer, 1, 13, 0.25f);

	Animation* playerAnim = mpAnimationManager->createAndManageAnimation(playerAnimData, 16);
	mpPlayerUnit = new Player(playerAnim, 200.0f, Vector2D(300, 300));

	mpGameListener = new GameListener();
	EventSystem::getInstance()->addListener(PLAYER_MOVE_EVENT, mpGameListener);
	EventSystem::getInstance()->addListener(KEYBOARD_EVENT, mpGameListener);
	EventSystem::getInstance()->addListener(MOUSE_EVENT, mpGameListener);

	mpGameTimer = new Timer();

	mTimePerFrame = 1.0f / fps;
	mDebugMode = debugMode;

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

	delete mpGameTimer;
	mpGameTimer = nullptr;
}

void Game::getInput()
{
	InputSystem::getInstance()->inputUpdate();
}

void Game::update()
{
	mpUnitManager->update(deltaTime);

	mpAnimationManager->update(deltaTime);

	mpPlayerUnit->setMoveDirection(InputSystem::getInstance()->getMovementAxis().normalized());
	mpPlayerUnit->update(deltaTime);
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
		cout << "Frame Length: " << deltaTime << ", which is equal to " << 1 / deltaTime << " FPS." << endl;
	}
}

void Game::DPlayerMove(Vector2D loc)
{
	cout << "Player move to: " << loc << endl;
}

void Game::DKeyPress(KeyCode key)
{
	cout << "Key pressed with ID: " << key << endl;
}

void Game::DMousePress(int button)
{
	cout << "Mouse Button pressed with ID: " << button << endl;
}

void Game::DKeyRelease(KeyCode key)
{
	cout << "Key released with ID: " << key << endl;
}

void Game::DMouseRelease(int button)
{
	cout << "Mouse Button released with ID: " << button << endl;
}

void Game::quitGame()
{
	mIsPlaying = false;
	cout << "QUIT" << endl;
}

void Game::fireProj()
{
	Vector2D dir = Vector2D(
		InputSystem::getInstance()->getMousePosition().getX() - mpPlayerUnit->getLocation().getX() + mpGraphicsSystem->getCameraLocation().getX(), 
		InputSystem::getInstance()->getMousePosition().getY() - mpPlayerUnit->getLocation().getY() + mpGraphicsSystem->getCameraLocation().getY());
	dir.normalize();

	AnimationData* projAnimData = mpAnimationManager->getAnimationData("proj");
	Animation* projAnim = mpAnimationManager->createAndManageAnimation(projAnimData, 13);

	mpUnitManager->createAndManageUnit(projAnim, mpPlayerUnit->getLocation(), dir, PROJECTILE_SPEED);
}