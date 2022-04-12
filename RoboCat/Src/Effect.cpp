#include "RoboCatPCH.h"
#include "Effect.h"

Effect::Effect(const int gameID, bool shouldDisplay) : GameObject{ GameObjectType::EFFECT, gameID }
{
	mShouldDisplay = shouldDisplay;
	mTimer = 0.0;
	mImageIdentifier = "effect";
}

Effect::Effect(const int gameID, float posX, float posY, bool shouldDisplay) : GameObject{ GameObjectType::EFFECT, gameID, posX, posY }
{
	mShouldDisplay = shouldDisplay;
	mTimer = 0.0;
	mImageIdentifier = "effect";
}

Effect::Effect(const int gameID, float posX, float posY, string imageIdentifier, bool shouldDisplay) : GameObject{ GameObjectType::EFFECT, gameID, posX, posY, imageIdentifier }
{
	mShouldDisplay = shouldDisplay;
	mTimer = 0.0;
	mImageIdentifier = "effect";
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


	inStream.Write(mGameObjType);
	inStream.Write(mIngameID);

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


	inStream.Read(mGameObjType);
	inStream.Read(mIngameID);

	inStream.Read(mPosX);
	inStream.Read(mPosY);

	inStream.Read(mShouldDisplay);
}

void Effect::SendEffect(TCPSocketPtr inSocket)
{
	if (inSocket)
	{
		OutputMemoryBitStream stream;
		Write(stream);
		inSocket->Send(stream.GetBufferPtr(), stream.GetBitLength());
	}
}

void Effect::ReceiveEffect(TCPSocketPtr inSocket)
{
	char buffer[1024];
	int32_t bytesReceived = inSocket->Receive(buffer, 1024);

	if (bytesReceived > 0)
	{
		InputMemoryBitStream stream(buffer, 1024);
		Read(stream);
	}

}

void Effect::Update(float dt)
{
	mTimer += dt;
}