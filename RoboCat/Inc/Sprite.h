#pragma once

class GraphicsBuffer;

class Sprite
{
	friend class GraphicsSystem;

public:

	Sprite();
	Sprite(GraphicsBuffer* gBuff, float locX, float locY, int width, int height);
	~Sprite();

	void init(GraphicsBuffer* gBuff, float locX, float locY, int width, int height);

	int getLocX() { return mLocX; }
	int getLocY() { return mLocY; }
	int getSpriteHeight() { return mSpriteHeight; }
	int getSpriteWidth() { return mSpriteWidth; }

private:
	GraphicsBuffer* mpGraphicsBuffer;
	float mLocX = 0.0f;
	float mLocY = 0.0f;
	int mSpriteWidth = 0;
	int mSpriteHeight = 0;

	GraphicsBuffer* getGraphicsBuffer() const { return mpGraphicsBuffer; }
};