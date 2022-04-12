#pragma once

enum GameObjectType
{
	PLAYER,
	BULLET,
	EFFECT,
	END_OF_GAME,
	ENUM_SIZE
};

class GameObject
{
protected:
	float mPosX = 0.0;
	float mPosY = 0.0;

public:

	int mIngameID = -1;

	float getX() { return mPosX; }
	float getY() { return mPosY; }

	void setX(float newX) { mPosX = newX; }
	void setY(float newY) { mPosY = newY; }

	GameObjectType mGameObjType = GameObjectType::ENUM_SIZE;



	string mImageIdentifier = "";
	GameObject(GameObjectType gameObjType, const int gameID);
	GameObject(GameObjectType gameObjType, const int gameID, float posX, float posY);
	GameObject(GameObjectType gameObjType, const int gameID, float posX, float posY, string imageIdentifier);

	~GameObject();

	const int getGameID() { return mIngameID; };
};