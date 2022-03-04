#pragma once

#include <raylib-cpp.hpp>
#include "Vector2D.h"
#include "Trackable.h"


class RColor;
class GraphicsBuffer;
class Sprite;
class RCamera2D;

class GraphicsSystem : public Trackable
{

public:
	GraphicsSystem();
	~GraphicsSystem();

	void init();
	void init(int width, int height, std::string title = "InfraRED");

	void cleanup();

	void clearScreenToColor(RColor);
	void drawText(std::string text, Vector2D loc, RColor col, int fontSize = 12);
	void draw(GraphicsBuffer* gb, Vector2D loc, float scale = 1.0f); //draw GraphicsBuffer to given location
	void draw(Sprite* sprite, Vector2D loc); //draw Sprite to given location

	int getScreenWidth() { return mWidth; }
	int getScreenHeight() { return mHeight; }

	void setCameraLocation(Vector2D location);
	Vector2D getCameraLocation();

	void flip();

	bool isRunning();

private:
	raylib::Window* mWindow;
	int mWidth, mHeight;

	RCamera2D* mCamera;

	bool mIsInit;

	raylib::Vector2 convertVector(Vector2D vec);

};
