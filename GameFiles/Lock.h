#pragma once
#include "GameObject.h"

class Lock : public GameObject
{
	const float MOVE_SPEED = 0.25;

	private:
		Lock();
		Lock(int posX, int posY);
		~Lock();
	protected:
	public:
		virtual void Update(WorldState* gameWorld, int delta);

		CLASS_IDENTIFICATION('LOCK', Lock)
};