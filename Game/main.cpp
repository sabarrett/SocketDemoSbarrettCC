/*
Author: Evan Koppers
Class: GPR-250 <Section 3>
Assignment: Assignment 5
Certification of Authenticity: I certify that this assignment is entirely my own work.
*/

#include <iostream>
#include <cassert>
#include <string>

#include <PerformanceTracker.h>
#include <MemoryTracker.h>
#include <string>

#include "Game.h"
#include "allegro5/allegro.h"
#include "EventSystem.h"
#include "GameEventSystem.h"
#include "GameListener.h"

using namespace std;

int main()
{
	EventSystem::initInstance();

	const double SLEEP_TIME = 5.0;

	PerformanceTracker* pPerformanceTracker = new PerformanceTracker;

	Game::initInstance();
	GameListener* gameListener = new GameListener();

	gameListener->init();

	Game::getInstance()->setFrameRate(60.0);
	Game::getInstance()->doLoop();
	Game::getInstance()->cleanUpInstance();

	gameListener->cleanup();
	delete(gameListener);

	EventSystem::getInstance()->cleanupInstance();

	delete pPerformanceTracker;

	MemoryTracker::getInstance()->reportAllocations(cout);
	system("pause");

	return 0;

}