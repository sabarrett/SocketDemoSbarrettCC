#include "RoboCatPCH.h"
#include "Player.h"

Player::Player(const int gameID, float speed) : GameObject{GameObjectType::PLAYER, gameID }
{
	mSpeed = speed;
	mImageIdentifier = "player";
}

Player::Player(const int gameID, float posX, float posY, float speed) : GameObject{ GameObjectType::PLAYER, gameID, posX, posY }
{
	mSpeed = speed;
	mImageIdentifier = "player";
}

Player::Player(const int gameID, float posX, float posY, float speed, string imageIdentifier) : GameObject{ GameObjectType::PLAYER, gameID, posX, posY, imageIdentifier }
{
	mSpeed = speed;
	mImageIdentifier = "player";
}

Player::~Player()
{

}

void Player::Write(OutputMemoryBitStream& inStream) const 
{
	// send game id: integer
	// send position: 2 floats
	// send if bullet was shot: bool
	// send if bullet hits: bool

	inStream.Write(mGameObjType);
	inStream.Write(mIngameID);

	inStream.Write(mPosX);
	inStream.Write(mPosY);

	inStream.Write(mIsFiring);
	inStream.Write(mIsHit);
}

void Player::Read(InputMemoryBitStream& inStream) 
{
	// receive game id: integer
	// receive position: 2 floats
	// receive if bullet was shot: bool
	// receive if bullet hits: bool

	inStream.Read(mGameObjType);
	inStream.Read(mIngameID);

	inStream.Read(mPosX);
	inStream.Read(mPosY);

	inStream.Read(mIsFiring);
	inStream.Read(mIsHit);
}

void Player::SendPlayer(TCPSocketPtr socketPtr)
{
	OutputMemoryBitStream stream;
	Write(stream);
	socketPtr->Send(stream.GetBufferPtr(), stream.GetBitLength());
	mIsFiring = false;
	//send(inSocket, stream.GetBufferPtr(), stream.GetBitLength(), 0);
}

void Player::ReceivePlayer(TCPSocketPtr inSocket)
{
	char buffer[1024];
	int32_t bytesReceived = inSocket->Receive(buffer, 1024);
	
	if (bytesReceived > 0)
	{
		InputMemoryBitStream stream(buffer, 1024);
		Read(stream);
	}
}

void Player::SetIsFiring(bool isFiring)
{
	mIsFiring = isFiring;
}

void Player::SetIsHit(bool isHit)
{
	mIsHit = isHit;
}

bool Player::GetIsFiring()
{
	return mIsFiring;
}

bool Player::GetIsHit()
{
	return mIsHit;
}

float Player::GetSpeed()
{
	return mSpeed;
}

void Player::Move(float deltaX)
{
	//std::cout << "delta = " << deltaX << std::endl;
	mPosX += deltaX;
}