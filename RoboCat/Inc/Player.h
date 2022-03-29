#pragma once
#include "GameObject.h"

class Player : public GameObject
{
private: 
	
	int mGameID;

	float mPosX;
	float mPosY;

	bool mIsFiring;
	bool mIsHit;

public:
	Player(const int gameID, const int netID);
	Player(const int gameID, const int netID, float posX, float posY);

	~Player();

	void Write(OutputMemoryBitStream& inStream)const;
	void Read(InputMemoryBitStream& inStream);

	void SendPlayer(int inSocket, const Player* inPlayer);
	void ReceivePlayer(int inSocket, Player* outPlayer);
};