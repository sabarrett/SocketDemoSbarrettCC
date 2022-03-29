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

	int mIngameID;

	float mPosX = 0.0;
	float mPosY = 0.0;

	GameObjectType mGameObjType;

public:

	string mImageIdentifier;
	GameObject(GameObjectType gameObjType, const int gameID);
	GameObject(GameObjectType gameObjType, const int gameID, float posX, float posY);
	GameObject(GameObjectType gameObjType, const int gameID, float posX, float posY, string imageIdentifier);

	~GameObject();

	const int getGameID() { return mIngameID; };

	float getPosX() { return mPosX; };
	void setPosX(float newPosX) { mPosX = newPosX; };

	float getPosY() { return mPosY; };
	void setPosY(float newPosY) { mPosY = newPosY; };
};