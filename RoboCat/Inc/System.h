#pragma once
//#include "GraphicsSystem.h"
#include "InputSystem.h"

class System 
{
public:
	System();
	~System();

	void init(int width, int height);
	void cleanup();

	//int getMouseLocX() {return mpI}

private:
	bool mIsInit;

	//GraphicsSys
}