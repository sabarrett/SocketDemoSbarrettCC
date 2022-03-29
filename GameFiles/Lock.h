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
		virtual void Update();

		CLASS_IDENTIFICATION('LOCK', Lock)
};