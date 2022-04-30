#pragma once
#include "Event.h"
#include <string>

class KeyEvent : public Event
{
public:
	KeyEvent(const string key);
	~KeyEvent();

	string getState() const { return mKey; };

private:
	string mKey;
};