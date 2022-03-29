#pragma once
#include "GameFiles/Lock.h"

class Key : public GameObject
{
private:
	Key();
	Key(int posX, int posY);
	~Key();
protected:
public:

	void Update();
	CLASS_IDENTIFICATION('Key', Key)
};