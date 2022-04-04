#pragma once

#include <string>
#include <utility>
#include "GraphicsLibrary.h"
#include "Colour.h"

using std::pair;

enum GameObjType
{
	BOULDER,
	BUBBLE,
	BEE,
	PLAYER,
	INVALID
};

class GameObjects
{
public:
	GameObjects() {};
	GameObjects(GameObjType type, GraphicsLibrary* gLib, const int networkID);
	GameObjects(GameObjType type, GraphicsLibrary* gLib, const int networkID, float posX, float posY, const std::string imgID = "");
	GameObjects(GameObjType type, GraphicsLibrary* gLib, const int networkID, float posX, float posY, const std::string imgID, int playerID);

	virtual ~GameObjects();

	const GameObjType getObjType() { return mObjType; }
	const int getNetworkID() { return mNetworkID; }
	const pair<float, float> getPosition() { return pair<float, float>(mPosX, mPosY); }

	void setPosition(float posX, float posY) { mPosX = posX; mPosY = posY; }
	void setPosX(float posX) { mPosX = posX; }
	void setPosY(float posY) { mPosY = posY; }
	std::string getImageID() { return mIMG_ID; }

	int getPlayerID() { return mPlayerID; }
	void setPlayerID(int ID) { mPlayerID = ID; }

	int getNum() { return mObjNum; }

	virtual void update() {};
	virtual void draw() {};

protected:

	GameObjType mObjType;
	const std::string mIMG_ID;
	int mNetworkID;
	float mPosX;
	float mPosY;
	int mPlayerID;

	int mObjNum;

	GraphicsLibrary* mGraphicsLib;
};


class Boulder : public GameObjects
{
public:
	
	Boulder(GraphicsLibrary* gLib, const int networkID);
	Boulder(GraphicsLibrary* gLib, const int networkID, const std::string imgID, float posX, float posY);
	~Boulder() {};
	void spawn(int posX, int posY) {};
	void drop() {};

	void update() {};
	void draw();

private:
	
	std::string mImageFileBoulder;
	int mXLoc;
	int mYLoc;
	int mBoulderSpeed;

};

class Bees : public GameObjects
{
public:

	Bees(GraphicsLibrary* gLib, const int networkID, const std::string imgID, float posX, float posY, int num);
	~Bees() {};
	void move() {};

	void setQuantity(int num) { mObjNum = num; }
	int getQuantity() { return mObjNum; }


	void update();
	void draw();

private:

	std::string mImageFileBee;
	int mBeeSpeed = 1;

};

class Bubble : public GameObjects
{
public:

	Bubble(GraphicsLibrary* gLib, const int networkID, const std::string imgID, float posX, float posY, int playerID);
	~Bubble() {};
	void spawn(std::string playerID) {};
	void pop() {};
	void move() {};

	void setPlayerColor(Colour color) { mBubbleColor = color; }
	Colour getPlayerColor() { return mBubbleColor; }

	void update();
	void draw();

private:

	std::string mImageFileBubble;
	Colour mBubbleColor;
	float mBubbleSpeed = 0.15;

};