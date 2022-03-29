#pragma once
#include "GameObject.h"
class Key :
	public GameObject
{
private:
	Key();
	Key(int posX, int posY);
	~Key();
protected:
public:
	virtual void Update();

	CLASS_IDENTIFICATION('KEYS', Key)
};

