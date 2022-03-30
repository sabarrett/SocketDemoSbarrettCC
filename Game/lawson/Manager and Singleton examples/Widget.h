#pragma once

#include <Trackable.h>

class Widget : public Trackable
{
	friend class WidgetManager;

public:
	inline float getContents() const {return mContents;};

private:
	float mContents;

	Widget( float contents ):mContents(contents){};
	~Widget(){};
};