#pragma once

using namespace std;

#include <vector>

#include "Sprite.h"
#include "Trackable.h"

class Animation : public Trackable
{
private:
	//Keeps track of sprites
	vector<Sprite> mSpriteList;
	int mCurrentSprite;
	bool mLoop;

	bool paused;

	//Keeps track of speed
	double mFrameTiming;
	double mUntilFrame;

public:
	Animation();
	Animation(double time, bool loop);
	~Animation();

	//Sprites
	void addSprite(Sprite newSprite);
	void deleteSprite(int pos);
	//Updates
	void update(double deltaTime);
	Sprite getCurrentSprite();
	//Timing
	void speedUp(int modify);
	void slowDown(int modify);
	void setTiming(double time);
	void setLooping(bool loop);
	void setPaused();
};