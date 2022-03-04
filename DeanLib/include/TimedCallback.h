#pragma once

#include <Trackable.h>

//template <class T, void (*F)(T)>
//void callbackFunc(T var)
//{
//	F(var);
//}

template <class T>
class TimedCallback :public Trackable
{
public:
	TimedCallback(double delayInMS, void(*func)(const T&), const T& val)
		:mDelayInMS(delayInMS)
		,mTimeLeft(delayInMS)
		,mpCallbackFunc(func)
		,mValue(val)
	{
	};

	bool update(double dtInMS)//returns true after calling callback, false otherwise
	{
		mTimeLeft -= dtInMS;
		if (mTimeLeft <= 0.0)
		{
			call(mValue);
			return true;
		}
		else
			return false;
	}

	void reset(double delayInMS)
	{
		mDelayInMS = delayInMS;
		mTimeLeft = delayInMS;
	}

	void setValue(const T& val)
	{
		mValue = val;
	}
private:
	void(*mpCallbackFunc)(const T&) = nullptr;
	double mDelayInMS = 0.0;
	double mTimeLeft = 0.0;
	T mValue;

	void call(const T& var)
	{
		mpCallbackFunc(var);
	};

};
