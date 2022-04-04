#pragma once

#include <string>
#include <utility>
#include "GraphicsLibrary.h"

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
	virtual ~GameObjects();

	const GameObjType getObjType() { return mObjType; }
	const int getNetworkID() { return mNetworkID; }
	const pair<float, float> getPosition() { return pair<float, float>(mPosX, mPosY); }

	void setPosition(float posX, float posY) { mPosX = posX; mPosY = posY; }
	std::string getImageID() { return mIMG_ID; }

	virtual void update() {};
	virtual void draw() {};

protected:

	GameObjType mObjType;
	const std::string mIMG_ID;
	int mNetworkID;
	float mPosX;
	float mPosY;

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

	Bees(GraphicsLibrary* gLib, const int networkID, const std::string imgID, float posX, float posY);
	~Bees() {};
	//void spawn(KeyCode direction) { };
	void move() {};

	void setDirection(char dir) { mDirection = dir; }
	char getDirection() { return mDirection; }

	void update() {};
	void draw();

private:

	std::string mImageFileBee;
	char mDirection;
	int mBeeQuantity;
	int mBeeSpeed;

};

class Bubble : public GameObjects
{
public:

	Bubble(GraphicsLibrary* gLib, const int networkID, const std::string imgID, float posX, float posY);
	~Bubble() {};
	void spawn(std::string playerID) {};
	void pop() {};
	void move() {};

	void setPlayerColor(Colour color) { mBubbleColor = color; }
	Colour getPlayerColor() { return mBubbleColor; }

	void update() {};
	void draw();

private:

	std::string mImageFileBubble;
	Colour mBubbleColor;
	int mBubbleSpeed;

};