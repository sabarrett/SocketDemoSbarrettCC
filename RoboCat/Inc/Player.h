#pragma once
#include "GameObject.h"

class Player : public GameObject
{
public:

	bool mIsFiring = false;
	bool mIsHit = false;
	float mSpeed = 0.02;

	Player(const int gameID, float speed);
	Player(const int gameID, float posX, float posY, float speed);
	Player(const int gameID, float posX, float posY, float speed, string imageIdentifier);

	~Player();

	void Write(OutputMemoryBitStream& inStream)const;
	void Read(InputMemoryBitStream& inStream);

	void SendPlayer(TCPSocketPtr inSocket);
	void ReceivePlayer(TCPSocketPtr inSocket);

	void SetIsFiring(bool isFiring);
	void SetIsHit(bool isHit);
	bool GetIsFiring();
	bool GetIsHit();
	float GetSpeed();

	void Move(float deltaX);
};