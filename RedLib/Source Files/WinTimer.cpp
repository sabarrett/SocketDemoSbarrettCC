#include "WinTimer.h"
#include "Windows.h"

Timer::Timer()
	:mElapsedTime(0.0)
	, mPaused(true)
{
	mStartTime = new LARGE_INTEGER();
	mEndTime = new LARGE_INTEGER();
	mTimerFrequency = new LARGE_INTEGER();

	QueryPerformanceFrequency(mTimerFrequency);
	mStartTime->QuadPart = 0;
	mEndTime->QuadPart = 0;
}

Timer::~Timer()
{
	delete mStartTime;
	delete mEndTime;
	delete mTimerFrequency;
	mStartTime = nullptr;
	mEndTime = nullptr;
	mTimerFrequency = nullptr;
}

void Timer::start()
{
	QueryPerformanceCounter(mStartTime);

	mEndTime->QuadPart = 0;
	mElapsedTime = 0.0;

	togglePause();
}

void Timer::stop()
{
	QueryPerformanceCounter(mEndTime);
	mElapsedTime = calcDifferenceInMS(mStartTime, mEndTime);
}

void Timer::togglePause()
{
	if (!mPaused)
	{
		mPaused = true;
		QueryPerformanceCounter(mEndTime);
		mElapsedTime += calcDifferenceInMS(mStartTime, mEndTime);
	}
	else
	{
		mPaused = false;
		QueryPerformanceCounter(mStartTime);
	}
}

double Timer::getElapsedTime() const
{
	if (mEndTime->QuadPart != 0)
	{
		return mElapsedTime;
	}
	else
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		return calcDifferenceInMS(mStartTime, &currentTime);
	}
}

void Timer::sleepUntilElapsed(double ms)
{
	LARGE_INTEGER currentTime, lastTime;
	QueryPerformanceCounter(&currentTime);
	double timeToSleep = ms - calcDifferenceInMS(mStartTime, &currentTime);
	
	while (timeToSleep > 0.0)
	{
		lastTime = currentTime;
		QueryPerformanceCounter(&currentTime);
		double timeElapsed = calcDifferenceInMS(&lastTime, &currentTime);
		timeToSleep -= timeElapsed;
		if (timeToSleep > 10.0)
		{
			Sleep(10);
		}
	}
}

double Timer::calcDifferenceInMS(LARGE_INTEGER* from, LARGE_INTEGER* to) const
{
	double difference = (double)(to->QuadPart - from->QuadPart) / (double)mTimerFrequency->QuadPart;
	return difference * 1000;
}
