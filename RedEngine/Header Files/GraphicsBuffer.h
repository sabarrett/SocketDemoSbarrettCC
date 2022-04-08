#pragma once
#include "LibraryIncludes.h"
#include "Trackable.h"
#include "Vector2D.h"

class GraphicsBuffer : public Trackable
{
public:
	friend class GraphicsSystem;
	friend class GraphicsBufferManager;

	void loadFromFile(std::string filename);

	void unload();

	int getWidth() { return mWidth; }
	int getHeight() { return mHeight; }

private:
	GraphicsBuffer() = delete;
	GraphicsBuffer(std::string filename);
	~GraphicsBuffer();

	raylib::Texture* mTexture;
	int mWidth, mHeight;

};