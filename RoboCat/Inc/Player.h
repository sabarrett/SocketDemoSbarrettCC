#pragma once
#include "GameObject.h"

class Player : public GameObject
{
private: 

	bool mIsFiring;
	bool mIsHit;

	float mSpeed;

public:
	Player(const int gameID, float speed);
	Player(const int gameID, float posX, float posY, float speed);
	Player(const int gameID, float posX, float posY, float speed, string imageIdentifier);

	~Player();

	void Write(OutputMemoryBitStream& inStream)const;
	void Read(InputMemoryBitStream& inStream);

	void SendPlayer(int inSocket, const Player* inPlayer);
	void ReceivePlayer(int inSocket, Player* outPlayer);
};