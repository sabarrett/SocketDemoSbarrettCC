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
protected:

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
	void setPosX(float newPosX) { mPosX = newPosX; };

	float getPosY() { return mPosY; };
	void setPosY(float newPosY) { mPosY = newPosY; };
};