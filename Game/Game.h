#pragma once

#include "System.h"

#include "Trackable.h"
#include "Timer.h"
#include "UnitManager.h"
#include "GraphicsBuffer.h"
#include "GraphicsBufferManager.h"
#include "UnitManager.h"
#include "InputTranslator.h"
#include "EventListener.h"

//Paths
const string DIRECTORY = "..\\..\\..\\..\\common\\assets\\";
const string WOODS_FILENAME = "Woods.png";
const string SMURF_SPRITES_FILENAME = "smurf_sprites.png";
const string DEAN_SPRITES_FILENAME = "dean_sprites.png";

//Sizes
const int SMURF_ROWS = 4;
const int SMURF_COLS = 4;
const int SMURF_SIZE = 60;

class Game : public Trackable
{
private:
	Game();
	~Game();

	//Instance of game
	static Game* mpsInstance;

	//Game Systems
	System * mpSystem;
	InputTranslator* mpInTranslator;

	//MouseX and Y
	int mUserX;
	int mUserY;

	//Time
	Timer mTimer;
	double mFrameRate;
	float  mSinceFrame;
	bool cont;

	//Managers
	GraphicsBufferManager* mpGManager;
	UnitManager* mpUnitManager;
	Animation mAnimation1;
	Animation mAnimation2;

public:
	
	void init();
	void cleanUp();
	void doLoop();
	void setFrameRate(double fps);

	//INSTANCE FUNCTIONS
	static Game* getInstance() { return mpsInstance; }
	static void initInstance();
	static void cleanUpInstance();

	//EVENT FUNCTIONS
	void escape();
	void pause();
	void switchAnim();
	void placeUnit(int x, int y);
	void deleteUnits(int x, int y);
};