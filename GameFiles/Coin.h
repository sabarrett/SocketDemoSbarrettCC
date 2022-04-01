#pragma once
#include "GameObject.h"
class Coin :
	public GameObject
{
private:
	Coin();
	Coin(int posX, int posY);
	~Coin();
protected:
public:
	virtual void Update(WorldState* gameWorld);

	CLASS_IDENTIFICATION('COIN', Coin)
};

