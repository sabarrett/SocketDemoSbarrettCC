//Inspired by Dean Lawson, Champlain College

#pragma once
#include "Trackable.h"

union _LARGE_INTEGER; //In Windows.h, LARGE_INTEGER and _LARGE_INTEGER are synonymous

class Timer :public Trackable
{
public:
	Timer();
	~Timer();

	void start();
	void stop();
	double getElapsedTime() const;
	void sleepUntilElapsed(double ms);
	void togglePause();

private:
	_LARGE_INTEGER* mStartTime;
	_LARGE_INTEGER* mEndTime;
	_LARGE_INTEGER* mTimerFrequency;
	double mElapsedTime;
	bool mPaused;

	//takes into account timer frequency
	double calcDifferenceInMS(_LARGE_INTEGER* from, _LARGE_INTEGER* to) const;

};