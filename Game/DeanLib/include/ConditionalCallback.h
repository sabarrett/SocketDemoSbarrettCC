#pragma once

#pragma once

#include <Trackable.h>

/*template <class T, void (*F)(T)>
void callbackFunc(T var)
{
	F(var);
}*/

template <class T>
class ConditionalCallback :public Trackable
{
public:
	ConditionalCallback(bool(*condFunc)(), void(*func)(T), T val)
		:mpCallbackFunc(func)
		, mpConditionalFunc(condFunc)
		, mValue(val)
	{
	};

	bool update(double dtInMS)//returns true after calling callback, false otherwise
	{
		if (mpConditionalFunc())
		{
			call(mValue);
			return true;
		}
		else
			return false;
	}

private:
	void(*mpCallbackFunc)(T) = nullptr;
	bool(*mpConditionalFunc)() = nullptr;
	T mValue;

	void call(T var)
	{
		mpCallbackFunc(var);
	};

};
