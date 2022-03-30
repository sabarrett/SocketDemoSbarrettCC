#include "Game.h"
#include "DeleteUnitsEvent.h"
#include "EscEvent.h"
#include "PauseEvent.h"
#include "PlaceUnitEvent.h"
#include "SwitchAnimEvent.h"

Game::Game()
{
	mUserY = 0;
	mUserX = 0;
	mFrameRate = 1000 / 60; //set default frame rate of 60fps
	mSinceFrame = 0;

	mpUnitManager = new UnitManager;
	mpSystem = new System;
	mpGManager = new GraphicsBufferManager;
	mpInTranslator = new InputTranslator;

	cont = true;
}

Game::~Game()
{
	cleanUp();
}

void Game::init()
{
	mpSystem->init(800, 600);

	//Add all buffers
	mpGManager->addGBuffer("woods", DIRECTORY + WOODS_FILENAME);
	mpGManager->addGBuffer("smurfs", DIRECTORY + SMURF_SPRITES_FILENAME);
	mpGManager->addGBuffer("dean", DIRECTORY + DEAN_SPRITES_FILENAME);

	//Create animations
	for (int i = 0; i < SMURF_ROWS; i++)
	{
		for (int j = 0; j < SMURF_COLS; j++)
		{
			mAnimation1.addSprite(Sprite(mpGManager->getGBuffer("smurfs"), { j * SMURF_SIZE, i * SMURF_SIZE }, SMURF_SIZE, SMURF_SIZE));
			mAnimation2.addSprite(Sprite(mpGManager->getGBuffer("dean"), { j * SMURF_SIZE, i * SMURF_SIZE }, SMURF_SIZE, SMURF_SIZE));
		}
	}

	//set animation variables
	mAnimation1.setLooping(true);
	mAnimation1.setTiming(150);
	mAnimation2.setTiming(150);
	mAnimation2.setLooping(true);

	mpUnitManager->addUnit(-1000, -1000, mAnimation1, mAnimation2);
}

void Game::cleanUp()
{
	mpInTranslator->cleanup();
	//delete(mpInTranslator);

	mpUnitManager->cleanup();
	delete mpUnitManager;

	mpGManager->cleanup();
	delete mpGManager;

	mpSystem->cleanup();
	delete(mpSystem);
}

void Game::doLoop()
{
	while (cont)
	{
		mTimer.start();

		//GET INPUT
		mpSystem->update();

		//ADJUST GAME STATE
		mpUnitManager->update(mSinceFrame); //units position and frame

		//RENDER
		mpSystem->mpGraphicsSystem->draw(mpGManager->getGBuffer("woods"), 0, 0, 0.5); //render woods
		mpUnitManager->draw(mpSystem->mpGraphicsSystem); //render unit

		//FLIP SCREEN
		mpSystem->mpGraphicsSystem->flip();

		//NEXT FRAME
		mTimer.sleepUntilElapsed(mFrameRate); //sleep until frame is over
		mSinceFrame = mTimer.getElapsedTime(); //get the time it took to complete this frame
		//cout << mSinceFrame << endl; //output that frame time
	}
}

void Game::setFrameRate(double fps)
{
	//set the frames per second, using 1000ms a second
	mFrameRate = 1000.0 / fps;
}

Game* Game::mpsInstance = NULL;

void Game::initInstance()
{
	if (mpsInstance == nullptr)
	{
		mpsInstance = new Game();
		mpsInstance->init();
	}
}

void Game::cleanUpInstance()
{
	delete mpsInstance;
	mpsInstance = nullptr;
}

void Game::escape()
{
	//exit the game
	cont = false;
}

void Game::pause()
{
	//pause all units animations
	mpUnitManager->pauseAnim();
}

void Game::switchAnim()
{
	//switch animation of last unit
	if (mpUnitManager->getUnitAnim(mpUnitManager->getLastUnit()) == 1)
	{
		mpUnitManager->setUnitAnim(mpUnitManager->getLastUnit(), 0);
	}
	else
	{
		mpUnitManager->setUnitAnim(mpUnitManager->getLastUnit(), 1);
	}
}

void Game::placeUnit(int x, int y)
{
	//add a unit
	mpUnitManager->addUnit(x - SMURF_SIZE / 2, y - SMURF_SIZE / 2, mAnimation1, mAnimation2);
	if (mpUnitManager->isPaused())
	{
		mpUnitManager->pauseAnim(mpUnitManager->getLastUnit());
	}
}

void Game::deleteUnits()
{
	//erase all units in a vicinity
	mpUnitManager->deleteRandomUnit();
}