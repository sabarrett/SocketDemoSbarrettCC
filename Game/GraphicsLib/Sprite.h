#pragma once

#include <allegro5/allegro.h>
#include "GraphicsBuffer.h"
#include <vector>
#include <string>
#include "Trackable.h"

using namespace std;

class Sprite : public Trackable
{
public:
	Sprite(GraphicsBuffer *image, vector<int> sourceLoc, int width, int height);
	Sprite();
	~Sprite();

	//getters
	vector<int> getSouceLoc();
	int getHeight();
	int getWidth();
	GraphicsBuffer getBuffer();

private:
	GraphicsBuffer* mpBuffer;
	vector<int> mSourceLoc;
	int mWidth, mHeight;
};