#pragma once
#include "GameObject.h"

class Bullet : public GameObject
{
private:

	bool mIsGoingUpwards = true;

	float mSpeed;


public:
	Bullet(const int gameID, float speed, bool isGoingUpwards);
	Bullet(const int gameID, float posX, float posY, float speed, bool isGoingUpwards);
	Bullet(const int gameID, float posX, float posY, string imageIdentifier, float speed, bool isGoingUpwards);

	~Bullet();

	void Write(OutputMemoryBitStream& inStream)const;
	void Read(InputMemoryBitStream& inStream);

	void SendBullet(TCPSocketPtr inSocket);
	void ReceiveBullet(TCPSocketPtr inSocket);

	void Update(float dt);

	bool gotDestroyed;
};