#include "RoboCatPCH.h"
#include "Player.h"

Player::Player(const int gameID, float speed) : GameObject{ gameID }
{
	mSpeed = speed;
}

Player::Player(const int gameID, float posX, float posY, float speed) : GameObject{ gameID, posX, posY }
{
	mSpeed = speed;
}

Player::Player(const int gameID, float posX, float posY, float speed, string imageIdentifier) : GameObject{ gameID, posX, posY, imageIdentifier }
{
	mSpeed = speed;
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

	inStream.Write(mGameID);

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

	inStream.Read(mGameID);

	inStream.Read(mPosX);
	inStream.Read(mPosY);

	inStream.Read(mIsFiring);
	inStream.Read(mIsHit);
}

void Player::SendPlayer(int inSocket, const Player* inPlayer)
{
	OutputMemoryBitStream stream;
	inPlayer->Write(stream);
	send(inSocket, stream.GetBufferPtr(), stream.GetBitLength(), 0);
}

void Player::ReceivePlayer(int inSocket, Player* outPlayer)
{
	char* temporaryBuffer = static_cast<char*>(std::malloc(kMaxPacketSize));
	size_t receivedBitCount = recv(inSocket, temporaryBuffer, kMaxPacketSize, 0);

	if (receivedBitCount > 0) {
		InputMemoryBitStream stream(temporaryBuffer,
			static_cast<uint32_t> (receivedBitCount));
		outPlayer->Read(stream);
	}
	else {
		std::free(temporaryBuffer);
	}

}