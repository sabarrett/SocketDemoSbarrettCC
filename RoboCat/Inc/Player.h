#pragma once
#include "GameObject.h"

class Player : public GameObject
{
public:
	Player(const int gameID, const int netID);
	Player(const int gameID, const int netID, float posX, float posY);

	~Player();
};