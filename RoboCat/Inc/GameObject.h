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
	public:

	int mIngameID = -1;

	float mPosX = 0.0;
	float mPosY = 0.0;

	GameObjectType mGameObjType = GameObjectType::ENUM_SIZE;



	string mImageIdentifier = "";
	GameObject(GameObjectType gameObjType, const int gameID);
	GameObject(GameObjectType gameObjType, const int gameID, float posX, float posY);
	GameObject(GameObjectType gameObjType, const int gameID, float posX, float posY, string imageIdentifier);

	~GameObject();

	const int getGameID() { return mIngameID; };
};