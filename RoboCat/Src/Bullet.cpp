#include "RoboCatPCH.h"
#include "Bullet.h"

Bullet::Bullet(const int gameID, float speed, bool isGoingUpwards) : GameObject{ GameObjectType::BULLET, gameID }
{
	mSpeed = speed;
	mIsGoingUpwards = isGoingUpwards;
	gotDestroyed = false;
	mImageIdentifier = "button";
}

Bullet::Bullet(const int gameID, float posX, float posY, float speed, bool isGoingUpwards) : GameObject{ GameObjectType::BULLET, gameID, posX, posY }
{
	mSpeed = speed;
	mIsGoingUpwards = isGoingUpwards;
	gotDestroyed = false;
	mImageIdentifier = "button";
}

Bullet::Bullet(const int gameID, float posX, float posY, string imageIdentifier, float speed, bool isGoingUpwards) : GameObject{ GameObjectType::BULLET, gameID, posX, posY, imageIdentifier }
{
	mSpeed = speed;
	mIsGoingUpwards = isGoingUpwards;
	gotDestroyed = false;
	mImageIdentifier = "button";
}

Bullet::~Bullet()
{

}

void Bullet::Write(OutputMemoryBitStream& inStream) const
{
	// send game id: integer
	// send position: 2 floats
	// send if bullet was shot: bool
	// send if bullet hits: bool

	inStream.Write(mGameObjType);
	inStream.Write(mIngameID);

	inStream.Write(mPosX);
	inStream.Write(mPosY);

	inStream.Write(mIsGoingUpwards);
	inStream.Write(gotDestroyed);
}

void Bullet::Read(InputMemoryBitStream& inStream)
{
	// receive game id: integer
	// receive position: 2 floats
	// receive if bullet was shot: bool
	// receive if bullet hits: bool


	inStream.Read(mGameObjType);
	inStream.Read(mIngameID);

	inStream.Read(mPosX);
	inStream.Read(mPosY);

	inStream.Read(mIsGoingUpwards);
	inStream.Read(gotDestroyed);
}

void Bullet::SendBullet(TCPSocketPtr inSocket)
{
	OutputMemoryBitStream stream;
	Write(stream);
	inSocket->Send(stream.GetBufferPtr(), stream.GetBitLength());
	//send(inSocket, stream.GetBufferPtr(), stream.GetBitLength(), 0);
}

void Bullet::ReceiveBullet(TCPSocketPtr inSocket)
{
	char buffer[1024];
	int32_t bytesReceived = inSocket->Receive(buffer, 1024);

	if (bytesReceived > 0)
	{
		InputMemoryBitStream stream(buffer, 1024);
		Read(stream);
	}
}

void Bullet::Update(float dt)
{
	if(mIsGoingUpwards)
		mPosY -= dt * mSpeed;
	else
		mPosY += dt * mSpeed;
}