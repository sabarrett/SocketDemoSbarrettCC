#include "Animation.h"
#include "Event.h"
#include "EventListener.h"
#include "EventSystem.h"
#include "Game.h"
#include "GameEvent.h"
#include "GraphicsBuffer.h"
#include "GraphicsBufferManager.h"
#include "InputSystem.h"
#include "InputTranslator.h"
#include "Sprite.h"
#include "System.h"
#include "Unit.h"
#include "UnitManager.h"

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 5
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

Game::Game()
{
	mpSystem = new System();
}

Game::~Game()
{
	cleanup();
}

void Game::initInstance()
{
	assert(!gpGame);
	gpGame = new Game;
}

Game* Game::getInstance()
{
	if (gpGame == nullptr)
	{
		gpGame = new Game;
	}
	
	return gpGame;
}

void Game::deleteInstance()
{
	delete gpGame;
	gpGame = nullptr;
}

void Game::init()
{
	EventSystem::initInstance();
	EventSystem::getInstance()->init();

	mpSystem->init();
	
	mpInputTranslator = new InputTranslator(mpSystem->getInputSystem());
	mpInputTranslator->init();

	EventSystem::getInstance()->addListener(EventType(CREATE_UNIT_EVENT), this);
	EventSystem::getInstance()->addListener(EventType(DESTROY_UNIT_EVENT), this);
	EventSystem::getInstance()->addListener(EventType(UPDATE_MOUSE_POSITION_EVENT), this);
	EventSystem::getInstance()->addListener(EventType(CHANGE_UNIT_ANIMATION_EVENT), this);
	EventSystem::getInstance()->addListener(EventType(TOGGLE_ALL_UNIT_ANIMATIONS_EVENT), this);
	EventSystem::getInstance()->addListener(EventType(EXIT_GAME_EVENT), this);

	//Create3GraphicsBuffers –one containing the woods.png image, one containing smurf_sprites.png, and one containing dean_sprites.pn
	mpWoodsBuffer = new GraphicsBuffer(ASSET_FILEPATH + WOODS_FILEPATH);
	mpSmurfBuffer = new GraphicsBuffer(ASSET_FILEPATH + SMURF_FILEPATH);
	mpDeanBuffer = new GraphicsBuffer(ASSET_FILEPATH + DEAN_FILEPATH);
	mpCurrentBuffer = mpSmurfBuffer;

	mpGraphicsBufferManager = new GraphicsBufferManager();

	mpGraphicsBufferManager->addBuffer("Smurf", mpSmurfBuffer);
	mpGraphicsBufferManager->addBuffer("Dean", mpDeanBuffer);
	mpGraphicsBufferManager->addBuffer("Woods", mpWoodsBuffer);

	// create Animations to hold sprites and Unit to hold animations
	mpWoodsSprite = new Sprite(mpWoodsBuffer, 0, 0, mpWoodsBuffer->getHeight(), mpWoodsBuffer->getWidth());

	mpUnitManager = new UnitManager();

	mAnimSpeed = 30;

	mShouldExit = false;
	mIsPaused = false;
	mIsSmurf = true;

	mpTimer = new Timer();
	mpPerformanceTracker = new PerformanceTracker();
}

void Game::cleanup()
{
	delete mpSystem;
	mpSystem = nullptr;

	delete mpDeanBuffer;
	mpDeanBuffer = nullptr;

	delete mpSmurfBuffer;
	mpSmurfBuffer = nullptr;

	delete mpWoodsSprite;
	mpWoodsSprite = nullptr;
	delete mpWoodsBuffer;
	mpWoodsBuffer = nullptr;

	delete mpDeanBuffer;
	mpDeanBuffer = nullptr;

	delete mpSmurfBuffer;
	mpSmurfBuffer = nullptr;

	delete mpGraphicsBufferManager;
	mpGraphicsBufferManager = nullptr;

	delete mpUnitManager;
	mpUnitManager = nullptr;

	delete mpInputTranslator;
	mpInputTranslator = nullptr;
	
	delete mpTimer;
	mpTimer = nullptr;

	delete mpPerformanceTracker;
	mpPerformanceTracker = nullptr;
}

void Game::doLoop()
{
	mpPerformanceTracker->startTracking( "doLoop" );

	do 
	{
		mpTimer->start();
		
		//The game loop
		mpSystem->getInputSystem()->update();  //<= Input System update throws an exception
		update();
		render();

		mpTimer->sleepUntilElapsed((1.0 / mFrameRate) * 1000); // wait for timer to reach ~16.7 milliseconds
		cout << mpTimer->getElapsedTime() << endl;
	} while (mShouldExit == false);

	mpPerformanceTracker->stopTracking("doLoop");
}

void Game::handleEvent(const Event& event)
{
	const GameEvent& myEvent = static_cast<const GameEvent&>(event);

	switch (myEvent.getType())
	{
	case CREATE_UNIT_EVENT:

		mpTempAnim = new Animation(4, 4, mpGraphicsBufferManager->getGraphicsBuffer("Smurf"), true);
		mpTempUnit = new Unit(mpUnitManager->getPosX() - 30, mpUnitManager->getPosY() - 30, mpTempAnim);

		mpTempUnit->setAnimationSpeed(30);
		mpUnitManager->addUnits(mpTempUnit);

		mIsSmurf = true;
		break;

	case DESTROY_UNIT_EVENT:

		for (int i = mpUnitManager->getNumUnits()-1; i >= 0; i--)
		{
			Unit* current = mpUnitManager->getUnits(i);
			if (mpUnitManager->getPosX() < current->getLocX() + current->getSpriteWidth() && mpUnitManager->getPosX() > current->getLocX())
			{
				if (mpUnitManager->getPosY() < current->getLocY() + current->getSpriteHeight() && mpUnitManager->getPosY() > current->getLocY())
				{
					mpUnitManager->deleteUnits(i);
					break;
				}
			}
		}
		break;

	case UPDATE_MOUSE_POSITION_EVENT:

		mpUnitManager->setPosX(myEvent.getPosX());
		mpUnitManager->setPosY(myEvent.getPosY());

		break;

	case CHANGE_UNIT_ANIMATION_EVENT:

		if (mIsSmurf)
		{
			mIsSmurf = false;
			mpUnitManager->getUnits(mpUnitManager->getNumUnits() - 1)->setAnimation(new Animation(4, 4, mpGraphicsBufferManager->getGraphicsBuffer("Smurf"), true));
			mpUnitManager->getUnits(mpUnitManager->getNumUnits() - 1)->setAnimationSpeed(30);
		}
		else
		{
			mIsSmurf = true;
			mpUnitManager->getUnits(mpUnitManager->getNumUnits() - 1)->setAnimation(new Animation(4, 4, mpGraphicsBufferManager->getGraphicsBuffer("Dean"), true));
			mpUnitManager->getUnits(mpUnitManager->getNumUnits() - 1)->setAnimationSpeed(30);
		}

		break;

	case TOGGLE_ALL_UNIT_ANIMATIONS_EVENT:

		if (!mIsPaused)
		{
			for (int i = 0; i < mpUnitManager->getNumUnits(); i++)
			{
				mpUnitManager->getUnits(i)->setAnimationSpeed(0);
			}
			mIsPaused = true;
		}
		else
		{
			for (int i = 0; i < mpUnitManager->getNumUnits(); i++)
			{
				mpUnitManager->getUnits(i)->setAnimationSpeed(mAnimSpeed);
			}
			mIsPaused = false;
		}

		break;

	case EXIT_GAME_EVENT:
		mShouldExit = true;
		break;
	}
}

void Game::update()
{
	mpUnitManager->updateUnits((1.0 / mFrameRate) * 1000);
}

void Game::render()
{
	mpSystem->getGraphicsSystem()->draw(0, 0, mpWoodsSprite, WOODS_SCALE);
	mpUnitManager->draw(mpSystem);
	mpSystem->getGraphicsSystem()->flip();
}

Game* Game::gpGame = nullptr;