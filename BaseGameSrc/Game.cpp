#include "Game.h"
#include "Unit.h"
#include "UnitManager.h"
#include "GraphicsBufferManager.h"

#include "MemoryManager.h"

#include <System.h>
#include <GraphicsSystem.h>
#include <GraphicsBuffer.h>
#include <Animation.h>

#include <timer.h>
#include <PerformanceTracker.h>
#include <iostream>

using namespace std;

Game* Game::mspInstance = NULL;

Game::Game()
{
	mpSystem = new System;
}

Game::~Game()
{
	cleanup();
	delete mpSystem;
}

Game* Game::getInstance()
{
	assert(mspInstance != NULL);
	return mspInstance;
}

void Game::initInstance()
{
	assert(mspInstance == NULL);
	if (mspInstance == NULL)
	{
		mspInstance = new Game();
	}
}

void Game::deleteInstance()
{
	assert(mspInstance != NULL);
	delete mspInstance;
	mspInstance = nullptr;
}

void Game::init(unsigned int width, unsigned int height, double targetTimePerFrame /*= 16.7*/)
{
	if (mIsInitted)
	{
		cleanup();
	}

	mpSystem->init(width, height);
	mTargetTimePerFrame = targetTimePerFrame;

	mpMemoryManager = new MemoryManager;
	mpMemoryManager->init();

	mpGraphicsBufferManager = new GraphicsBufferManager;
	mpUnitManager = new UnitManager;
	
	

	loadBuffers();

	mIsInitted = true;
}

void Game::cleanup()
{
	if (mIsInitted)
	{
		
		
		delete mpUnitManager;
		delete mpGraphicsBufferManager;
		delete mpMemoryManager;
		
		mpSystem->cleanup();
	}

	mIsInitted = false;
}

void Game::doLoop()
{
	Timer timer;
	PerformanceTracker tracker;
	
	while (mShouldContinue)
	{
		tracker.clearTracker("loop");
		tracker.startTracking("loop");
		timer.start();

		getInput();
		update(mTargetTimePerFrame);
		render();

		timer.sleepUntilElapsed(mTargetTimePerFrame);
		tracker.stopTracking("loop");
		//cout << tracker.getElapsedTime("loop") << endl;

		cout << "Allocated memory: " << mpMemoryManager->getTotalAllocated() << endl;
		//cout << "Current available memory: " << mpMemoryManager->getTotalCapacity() << endl;
	}
}

void Game::getInput()
{
	if (mpSystem->isKeyPressed(System::ESCAPE_KEY))
	{
		mShouldContinue = false;
	}
	if (mpSystem->isKeyPressed(System::ENTER_KEY))
	{
		Unit* pUnit = mpUnitManager->getLastUnitCreated();
		if (pUnit)
		{
			pUnit->toggleAnimation();
		}
	}
	if (mpSystem->isKeyPressed(System::SPACE_KEY))
	{
		mpUnitManager->togglePauseStateForAllAnimations();
	}
	if (mpSystem->isMouseButtonPressed(System::LEFT))
	{
		Vector2D mousePos = mpSystem->getCurrentMousePos();
		createUnit(mousePos);
	}
	if (mpSystem->isMouseButtonPressed(System::RIGHT))
	{
		Vector2D mousePos = mpSystem->getCurrentMousePos();
		mpUnitManager->deleteAllUnitsAt2DPosition(mousePos);
	}
}

void Game::update(double dt)
{
	mpUnitManager->update(dt);
}

void Game::render()
{
	//draw background
	GraphicsSystem::draw(ZERO_VECTOR2D, *(mpGraphicsBufferManager->getBuffer(WOODS)), 0.5f);

	//draw units
	mpUnitManager->draw();

	mpSystem->getGraphicsSystem()->flip();
}

void Game::loadBuffers()
{
	const string ASSET_PATH = "..\\..\\shared\\assets\\";
	const string BACKGROUND_FILENAME = "woods.png";
	const string SMURF_FILENAME = "smurf_sprites.png";
	const string DEAN_FILENAME = "dean_sprites.png";

	mpGraphicsBufferManager->createBuffer(WOODS, ASSET_PATH + BACKGROUND_FILENAME);
	mpGraphicsBufferManager->createBuffer(SMURFS, ASSET_PATH + SMURF_FILENAME);
	mpGraphicsBufferManager->createBuffer(DEAN, ASSET_PATH + DEAN_FILENAME);

}

const int PIXEL_WIDTH = 60;
const int PIXEL_HEIGHT = 60;
const int SPRITES_ACROSS = 4;
const int SPRITES_DOWN = 4;
const float TIME_PER_FRAME_MULTIPLE = 5;

void Game::createUnit(const Vector2D& pos)
{
	float timePerFrame = (float)mTargetTimePerFrame * TIME_PER_FRAME_MULTIPLE;
	const GraphicsBuffer* pSmurfs = mpGraphicsBufferManager->getBuffer(SMURFS);
	assert(pSmurfs);
	const GraphicsBuffer* pDean = mpGraphicsBufferManager->getBuffer(DEAN);
	Animation smurfAnimation(*pSmurfs, PIXEL_WIDTH, PIXEL_HEIGHT, SPRITES_ACROSS, SPRITES_DOWN, timePerFrame);
	Animation deanAnimation(*pDean, PIXEL_WIDTH, PIXEL_HEIGHT, SPRITES_ACROSS, SPRITES_DOWN, timePerFrame);

	mpUnitManager->createUnit(pos, smurfAnimation, deanAnimation);
}

