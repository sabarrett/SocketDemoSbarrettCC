#pragma once
#include "GameObject.h"
class Key :
	public GameObject
{
private:
	const float MOVE_SPEED = 0.1f;

	Key();
	Key(int posX, int posY);
	~Key();
protected:
public:
	virtual void Update(WorldState* gameWorld, int delta);

	CLASS_IDENTIFICATION('KEYS', Key)
};

