#pragma once

class GameObject
{
	const int mGameID;
	const int mNetID;

	float mPosX;
	float mPosY;
public:
	GameObject(const int gameID, const int netID);
	GameObject(const int gameID, const int netID, float postX, float PosY);

	~GameObject();

	const int getGameID() { return mGameID; };
	const int getNetId() { return mNetID; };
};