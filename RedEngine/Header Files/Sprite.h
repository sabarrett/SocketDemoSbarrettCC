#pragma once
#include "Trackable.h"
#include "Vector2D.h"

class GraphicsBuffer;

class Sprite : public Trackable
{
public:
	friend class GraphicsSystem;

	Sprite(GraphicsBuffer* gb, Vector2D loc, Vector2D size, float scale = 1.0f);
	~Sprite();

	Vector2D getSize() { return mSize; }
	Vector2D getLocation() { return mLoc; }
	float getScale() { return mScale; }
	
private:
	Sprite() = delete;

	GraphicsBuffer* mpGraphicsBuffer;
	Vector2D mLoc, mSize;
	float mScale;
};
