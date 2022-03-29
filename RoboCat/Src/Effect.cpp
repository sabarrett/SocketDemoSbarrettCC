#include "RoboCatPCH.h"
#include "Effect.h"

Effect::Effect(const int gameID, bool shouldDisplay) : GameObject{ gameID }
{
	mShouldDisplay = shouldDisplay;
}

Effect::Effect(const int gameID, float posX, float posY, bool shouldDisplay) : GameObject{ gameID, posX, posY }
{
	mShouldDisplay = shouldDisplay;
}

Effect::~Effect()
{

}

void Effect::Write(OutputMemoryBitStream& inStream) const
{
	// send game id: integer
	// send position: 2 floats
	// send if bullet was shot: bool
	// send if bullet hits: bool

	inStream.Write(mGameID);

	inStream.Write(mPosX);
	inStream.Write(mPosY);

	inStream.Write(mShouldDisplay);
}

void Effect::Read(InputMemoryBitStream& inStream)
{
	// receive game id: integer
	// receive position: 2 floats
	// receive if bullet was shot: bool
	// receive if bullet hits: bool

	inStream.Read(mGameID);

	inStream.Read(mPosX);
	inStream.Read(mPosY);

	inStream.Read(mShouldDisplay);
}

void Effect::SendPlayer(int inSocket, const Effect* inEffect)
{
	OutputMemoryBitStream stream;
	inEffect->Write(stream);
	send(inSocket, stream.GetBufferPtr(), stream.GetBitLength(), 0);
}

void Effect::ReceivePlayer(int inSocket, Effect* outEffect)
{
	char* temporaryBuffer = static_cast<char*>(std::malloc(kMaxPacketSize));
	size_t receivedBitCount = recv(inSocket, temporaryBuffer, kMaxPacketSize, 0);

	if (receivedBitCount > 0) {
		InputMemoryBitStream stream(temporaryBuffer,
			static_cast<uint32_t> (receivedBitCount));
		outEffect->Read(stream);
	}
	else {
		std::free(temporaryBuffer);
	}

}