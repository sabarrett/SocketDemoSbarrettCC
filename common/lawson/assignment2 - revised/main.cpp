#include <iostream>
#include <cassert>
#include <string>

#include <PerformanceTracker.h>
#include <MemoryTracker.h>

#include <GraphicsLib.h>
#include <allegro5/allegro.h>


using namespace std;

int main()
{
	const double SLEEP_TIME = 0.0;

	PerformanceTracker* pPerformanceTracker = new PerformanceTracker;

	const string INIT_TRACKER_NAME = "init";
	const string DRAW_TRACKER_NAME = "draw";
	const string WAIT_TRACKER_NAME = "wait";

	pPerformanceTracker->startTracking(INIT_TRACKER_NAME);
	ALLEGRO_DISPLAY* pDisplay = initAllegro();
	pPerformanceTracker->stopTracking(INIT_TRACKER_NAME);


	pPerformanceTracker->startTracking(DRAW_TRACKER_NAME);
	drawScene(pDisplay);
	pPerformanceTracker->stopTracking(DRAW_TRACKER_NAME);

	Timer* pTimer = new Timer;

	pPerformanceTracker->startTracking(WAIT_TRACKER_NAME);
	pTimer->start();
	pTimer->sleepUntilElapsed(SLEEP_TIME * 1000.0);
	pPerformanceTracker->stopTracking(WAIT_TRACKER_NAME);

	//event queue stuff starts here!!!!

	ALLEGRO_EVENT_QUEUE* pQueue = initEventQueue();

	//this is our fake game loop
	Timer theTimer;
	bool shouldStop = false;
	while (!shouldStop)
	{
		theTimer.start();
		shouldStop = processEventQueue(pQueue);
		theTimer.sleepUntilElapsed(16.7);
	}
	
	cleanupEventQueue(pQueue);

	//event queue stuff stops here!!!!


	cleanupAllegro(pDisplay);

	//report elapsed times
	cout << endl << "Time to Init:" << pPerformanceTracker->getElapsedTime(INIT_TRACKER_NAME) << " ms" << endl;
	cout << endl << "Time to Draw:" << pPerformanceTracker->getElapsedTime(DRAW_TRACKER_NAME) << " ms" << endl;
	cout << endl << "Time to Wait:" << pPerformanceTracker->getElapsedTime(WAIT_TRACKER_NAME) << " ms" << endl;

	MemoryTracker::getInstance()->reportAllocations(cout);
	system("pause");

	return 0;

}