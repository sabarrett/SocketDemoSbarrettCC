#pragma once

#include "Timer.h"

#include <map>
#include <string>
#include "Trackable.h"

class PerformanceTracker: public Trackable
{
public:
	PerformanceTracker();
	~PerformanceTracker();

	void startTracking( const std::string& trackerName );
	void stopTracking( const std::string& trackerName );
	double getElapsedTime( const std::string& trackerName );
	void removeTracker( const std::string& trackerName );
	void clearTracker( const std::string& trackerName );

private:
	std::map<std::string,Timer*> mTimers;
};

