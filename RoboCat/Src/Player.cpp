#include "RoboCatPCH.h"
#include "Player.h"

Player::Player(const int gameID, const int netID) : GameObject(gameID, netID)
{

}

Player::Player(const int gameID, const int netID, float posX, float posY) : GameObject(gameID, netID, posX, posY)
{

}

Player::~Player()
{

}