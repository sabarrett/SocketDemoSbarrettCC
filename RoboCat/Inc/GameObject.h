#pragma once

enum GameObjectType
{
	PLAYER,
	BULLET,
	EFFECT,
	ENUM_SIZE
};

class GameObject
{
private:
	const int mGameID;

	float mPosX;
	float mPosY;


public:

	string mImageIdentifier;
	GameObject(const int gameID);
	GameObject(const int gameID, float posX, float posY);
	GameObject(const int gameID, float posX, float posY, string imageIdentifier);

	~GameObject();

	const int getGameID() { return mGameID; };

	float getPosX() { return mPosX; };
	float setPosX(float newPosX) { mPosX = newPosX; };

	float getPosY() { return mPosY; };
	float setPosY(float newPosY) { mPosY = newPosY; };
};