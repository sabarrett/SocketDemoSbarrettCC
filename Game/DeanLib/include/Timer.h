#pragma once
#include <windows.h>
#include "Trackable.h"

/* Timer - high accuracy timer - uses Large Integer to prevent rollover

Dean Lawson
Champlain College
2011
*/

class Timer:public Trackable
{
public:
	Timer();
	~Timer();

	void start();
	void stop();
	double getElapsedTime() const;//returns how much time has elapsed since start
	void sleepUntilElapsed( double ms );
	void pause( bool shouldPause );

private:
	LARGE_INTEGER mStartTime;
	LARGE_INTEGER mEndTime;
	LARGE_INTEGER mTimerFrequency;
	double mElapsedTime;
	bool mPaused;

	//helper function - uses current frequency for the Timer
	double calcDifferenceInMS( LARGE_INTEGER from, LARGE_INTEGER to ) const;

};