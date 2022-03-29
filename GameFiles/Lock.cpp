
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

void Lock::Write(OutputMemoryBitStream& stream)
{
	stream.Write(mGraphicsData);
	stream.Write(mpGraphicsID);
}

void Lock::Read(InputMemoryBitStream& stream)
{
	stream.Read(mGraphicsData);
	stream.Read(mpGraphicsID);
}

void Lock::Update()
{
	if (mGraphicsData.mX > 1500) mGraphicsData.mX = 0;
	mGraphicsData.mX++;
}
