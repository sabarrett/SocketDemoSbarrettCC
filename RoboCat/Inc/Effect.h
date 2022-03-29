#pragma once
#include "GameObject.h"

class Effect : public GameObject
{
private:

	bool mShouldDisplay;

public:
	float mTimer;
	float mMaxTime;

	Effect(const int gameID, bool shouldDisplay);
	Effect(const int gameID, float posX, float posY, bool shouldDisplay);
	Effect(const int gameID, float posX, float posY, string imageIdentifier, bool shouldDisplay);

	~Effect();

	void Write(OutputMemoryBitStream& inStream)const;
	void Read(InputMemoryBitStream& inStream);

	void SendPlayer(int inSocket, const Effect* inEffect);
	void ReceivePlayer(int inSocket, Effect* outEffect);

	void Update(float dt);
};