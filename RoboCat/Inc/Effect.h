#pragma once
#include "GameObject.h"

class Effect : public GameObject
{
public:

	bool mShouldDisplay = true;
	Effect(const int gameID, bool shouldDisplay);
	Effect(const int gameID, float posX, float posY, bool shouldDisplay);
	Effect(const int gameID, float posX, float posY, string imageIdentifier, bool shouldDisplay);

	~Effect();

	void Write(OutputMemoryBitStream& inStream)const;
	void Read(InputMemoryBitStream& inStream);

	void SendEffect(TCPSocketPtr inSocket);
	void ReceiveEffect(TCPSocketPtr inSocket);

	void Update(float dt);

	float mTimer = 0.0;
};