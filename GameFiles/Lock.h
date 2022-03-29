#pragma once
#include "GameObject.h"

class Lock : public GameObject
{
	private:
		Lock();
		Lock(int posX, int posY);
		~Lock();
	protected:
	public:

		void Update();
		CLASS_IDENTIFICATION('LOCK', Lock)
};