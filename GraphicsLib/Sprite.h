#pragma once

#include <Trackable.h>
#include <Vector2D.h>

class GraphicsBuffer;

class Sprite : public Trackable
{
public:
	Sprite(const GraphicsBuffer* pBuffer, const Vector2D& srcLoc, int width, int height);
	Sprite() {};

	const GraphicsBuffer* getBuffer() const { return mpBuffer; };
	const Vector2D& getSourceLoc() const { return mSrcLoc; };
	int getWidth() const { return mWidth; };
	int getHeight() const { return mHeight; };
	Vector2D getCenterOffset() const { return Vector2D(mWidth / 2, mHeight / 2); };
private:
	const GraphicsBuffer* mpBuffer = NULL;
	Vector2D mSrcLoc = ZERO_VECTOR2D;
	int mWidth = 0;
	int mHeight = 0;
};