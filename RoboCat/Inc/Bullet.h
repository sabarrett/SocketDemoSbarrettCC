#pragma once
#include "GameObject.h"

class Bullet : public GameObject
{
private:

	bool mIsGoingUpwards;

	float mSpeed;

public:
	Bullet(const int gameID, float speed, bool isGoingUpwards);
	Bullet(const int gameID, float posX, float posY, float speed, bool isGoingUpwards);

	~Bullet();

	void Write(OutputMemoryBitStream& inStream)const;
	void Read(InputMemoryBitStream& inStream);

	void SendBullet(int inSocket, const Bullet* inBullet);
	void ReceiveBullet(int inSocket, Bullet* outBullet);
};