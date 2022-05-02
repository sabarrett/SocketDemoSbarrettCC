#include "Singleton.h"
#include <cassert>

Singleton* Singleton::mpsInstance = NULL;

Singleton* Singleton::getInstance()
{
	if (mpsInstance == NULL)
		mpsInstance = new Singleton;
	return mpsInstance;
}

/*void Singleton::cleanup()
{ 
	delete mpsInstance; 
	mpsInstance = NULL; 
};

void Singleton::init()
{ 
	if (mpsInstance == NULL)
	{
		mpsInstance = new Singleton;
	}
}*/