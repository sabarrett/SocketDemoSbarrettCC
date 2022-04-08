//Inspired by Dean Lawson, Champlain College

#pragma once
#include "Trackable.h"
#include <chrono>

const double NANO_TO_SEC = 1000000000.0;
//const double MILLI_TO_SEC = 1000.0;
const double NANO_TO_MILLI = 1000000.0;

class Timer : public Trackable
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
	std::chrono::steady_clock::time_point mStartTime;
	std::chrono::steady_clock::time_point mEndTime;
	double mElapsedTime;
	bool mPaused;

};