#include "RoboCatPCH.h"
#include "Bullet.h"

Bullet::Bullet(const int gameID, float speed, bool isGoingUpwards) : GameObject{ gameID }
{
	mSpeed = speed;
	mIsGoingUpwards = isGoingUpwards;
}

Bullet::Bullet(const int gameID, float posX, float posY, float speed, bool isGoingUpwards) : GameObject{ gameID, posX, posY }
{
	mSpeed = speed;
	mIsGoingUpwards = isGoingUpwards;
}

Bullet::Bullet(const int gameID, float posX, float posY, string imageIdentifier, float speed, bool isGoingUpwards) : GameObject{ gameID, posX, posY, imageIdentifier }
{
	mSpeed = speed;
	mIsGoingUpwards = isGoingUpwards;
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

	inStream.Write(mGameID);

	inStream.Write(mPosX);
	inStream.Write(mPosY);

	inStream.Write(mIsGoingUpwards);
}

void Bullet::Read(InputMemoryBitStream& inStream)
{
	// receive game id: integer
	// receive position: 2 floats
	// receive if bullet was shot: bool
	// receive if bullet hits: bool

	inStream.Read(mGameID);

	inStream.Read(mPosX);
	inStream.Read(mPosY);

	inStream.Read(mIsGoingUpwards);
}

void Bullet::SendBullet(int inSocket, const Bullet* inBullet)
{
	OutputMemoryBitStream stream;
	inBullet->Write(stream);
	send(inSocket, stream.GetBufferPtr(), stream.GetBitLength(), 0);
}

void Bullet::ReceiveBullet(int inSocket, Bullet* outBullet)
{
	char* temporaryBuffer = static_cast<char*>(std::malloc(kMaxPacketSize));
	size_t receivedBitCount = recv(inSocket, temporaryBuffer, kMaxPacketSize, 0);

	if (receivedBitCount > 0) {
		InputMemoryBitStream stream(temporaryBuffer,
			static_cast<uint32_t> (receivedBitCount));
		outBullet->Read(stream);
	}
	else {
		std::free(temporaryBuffer);
	}
}

void Bullet::Update(float dt)
{
	if(mIsGoingUpwards)
		mPosY -= dt * mSpeed;
	else
		mPosY += dt * mSpeed;
}