
#include "RoboCatPCH.h"
#include "Lock.h"


Lock::Lock() : GameObject("lock_blue.png", 70, 70)
{

}

Lock::Lock(int posX, int posY) : GameObject("lock_blue.png", posX, posY, 70, 70)
{

}

Lock::~Lock()
{
}

void Lock::Update()
{
}
