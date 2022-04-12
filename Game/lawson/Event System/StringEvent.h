#pragma once

#include <string>
#include "Event.h"

using namespace std;

class StringEvent : public Event
{
public:
	StringEvent( const string& theString );
	~StringEvent();

	inline string getString() const { return mString; };

private:
	string mString;
};