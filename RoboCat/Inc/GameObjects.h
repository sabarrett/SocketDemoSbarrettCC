#pragma once

#include <string>
#include <utility>
#include "GraphicsLibrary.h"
#include "GameController.h"

using std::pair;

enum CurrentObject
{
	BOULDER,
	BUBBLE,
	BEE,
	INVALID
};

class GameObjects
{
public:
	GameObjects() {};
	GameObjects(CurrentObject type, GraphicsLibrary* gLib, const int networkID);
	GameObjects(CurrentObject type, GraphicsLibrary* gLib, const int networkID, float posX, float posY, const std::string imgID = "");
	virtual ~GameObjects();

	const CurrentObject getObjType() { return mObjType; }
	const int getNetworkID() { return mNetworkID; }
	const pair<float, float> getPosition() { return pair<float, float>(mPosX, mPosY); }

	void setPosition(float posX, float posY) { mPosX = posX; mPosY = posY; }

	virtual void update() {};
	virtual void draw() {};

protected:

	CurrentObject mObjType;
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
	void spawn(KeyCode direction) {};
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