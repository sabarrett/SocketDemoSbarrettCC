#include "RoboCatPCH.h"
#include "PlayerMarcher.h"

PlayerMarcher::PlayerMarcher() : GameObject("hud_p1.png", 47, 47)
{
	mIsServer = true;
	mVelocity = Vector3();
	SetGoalPos(0, 0, true);
}

PlayerMarcher::PlayerMarcher(bool isServer) : GameObject("hud_p1.png", 47, 47)
{
	mIsServer = isServer;
	mVelocity = Vector3();
	SetGoalPos(0, 0, isServer);
}

PlayerMarcher::PlayerMarcher(bool isServer, int posX, int posY) : GameObject("hud_p1.png", posX, posY, 47, 47)
{
	mIsServer = isServer;
	mVelocity = Vector3();
	SetGoalPos(posX, posY, isServer);
}

PlayerMarcher::~PlayerMarcher()
{
}

void PlayerMarcher::Update(WorldState* gameWorld, int delta)
{
	Vector3 force = mGoal - GetPosition();
	if (force.LengthSq() > MAX_FORCE * MAX_FORCE)
	{
		force.Normalize();
		force *= MAX_FORCE;
	}
	mVelocity += force * delta;
	if (mVelocity.LengthSq() > MAX_SPEED * MAX_SPEED)
	{
		mVelocity.Normalize();
		mVelocity *= MAX_SPEED;
	}

	mGraphicsData.mX += mVelocity.mX * delta;
	mGraphicsData.mY += mVelocity.mY * delta;

}

void PlayerMarcher::SetGoalPos(int posX, int posY, bool isServer)
{
	if (mIsServer == isServer)
	{
		mGoal = Vector3(posX, posY, 0);
	}
}

void PlayerMarcher::SetIsServer(bool isServer)
{
	mIsServer = isServer;
}

void PlayerMarcher::Write(OutputMemoryBitStream& stream)
{
	stream.Write(mGraphicsData.mX);
	stream.Write(mGraphicsData.mY);
	stream.Write(mGraphicsData.mZ);
	stream.Write(mGraphicsData.mW);
	stream.Write(*mpGraphicsID);
	stream.Write(mGoal);
	stream.Write(mVelocity);
	stream.Write(mIsServer);
}

void PlayerMarcher::Read(InputMemoryBitStream& stream)
{
	stream.Read(mGraphicsData.mX);
	stream.Read(mGraphicsData.mY);
	stream.Read(mGraphicsData.mZ);
	stream.Read(mGraphicsData.mW);
	stream.Read(*mpGraphicsID);
	stream.Read(mGoal);
	stream.Read(mVelocity);
	stream.Read(mIsServer);
}
