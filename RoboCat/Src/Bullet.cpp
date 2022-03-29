#include "RoboCatPCH.h"
#include "Bullet.h"

Bullet::Bullet(const int gameID, const int netID) : GameObject{gameID, netID}
{
	
}

Bullet::Bullet(const int gameID, const int netID, float posX, float posY) : GameObject{ gameID, netID, posX, posY }
{

}

void Bullet::update()
{
	//pos
}