#include "PerformanceTracker.h"

using namespace std;

PerformanceTracker::PerformanceTracker()
{
}

PerformanceTracker::~PerformanceTracker()
{
	map<string,Timer*>::iterator iter;
	for( iter = mTimers.begin(); iter != mTimers.end(); ++iter )
	{
		delete iter->second;
	}
}

void PerformanceTracker::startTracking( const string& trackerName )
{
	Timer* pTimer = NULL;

	//find if tracker already exists
	map<string,Timer*>::iterator iter = mTimers.find( trackerName );
	if( iter != mTimers.end() )
	{
		pTimer = iter->second;
	}
	else
	{
		pTimer = new Timer();
		pTimer->start();
		mTimers[trackerName] = pTimer;
	}

	pTimer->pause(false);
}

void PerformanceTracker::stopTracking( const string& trackerName )
{
	//make sure timer already exists
	map<string,Timer*>::iterator iter = mTimers.find( trackerName );
	if( iter != mTimers.end() )
	{
		iter->second->pause(true);
	}

}

double PerformanceTracker::getElapsedTime( const string& trackerName )
{
	//make sure timer already exists
	map<string,Timer*>::iterator iter = mTimers.find( trackerName );
	if( iter != mTimers.end() )
	{
		return iter->second->getElapsedTime();
	}
	else
		return 0.0;
}

void PerformanceTracker::removeTracker( const std::string& trackerName )
{
	//find the existing timer
	map<string,Timer*>::iterator iter = mTimers.find( trackerName );
	if( iter != mTimers.end() )
	{
		delete iter->second;
		mTimers.erase( iter );
	}

}

void PerformanceTracker::clearTracker( const std::string& trackerName )
{
	//find if tracker already exists
	map<string,Timer*>::iterator iter = mTimers.find( trackerName );
	if( iter != mTimers.end() )
	{
		iter->second->start();
	}
}