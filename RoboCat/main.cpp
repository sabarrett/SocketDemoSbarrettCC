#include <iostream>
#include <cassert>
#include <string>

#include <PerformanceTracker.h>
#include <MemoryTracker.h>

#include"Color.h"
#include"EventSystem.h"
#include"Font.h"
#include"Game.h"
#include"GraphicsBuffer.h"
#include"GraphicsSystem.h"
#include"Sprite.h"
#include"System.h"
#include <allegro5/allegro.h>

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 4
	Certification of Authenticity :
I certify that this assignment is entirely our own work.
*/

using namespace std;

const int DISPLAY_WIDTH = 800;
const int DISPLAY_HEIGHT = 600;

int main()
{
	Game::initInstance();
	Game* instance = Game::getInstance();

	instance->init();
	
	instance->doLoop();

	instance->cleanup();
	Game::deleteInstance();
	EventSystem::cleanupInstance();

	cout << endl;

	MemoryTracker::getInstance()->reportAllocations( cout );
	system( "pause" );
	

	return 0;
}