#pragma once
#include "Trackable.h"
#include <vector>

class Sprite;
class GraphicsBuffer;
class AnimationData;

class Animation : public Trackable
{

public:
	friend class AnimationManager;

	void update(double deltaTime);

	Sprite* getCurrentSprite();

private:
	Animation() = delete;
	Animation(AnimationData* data, int fps);
	~Animation();

	AnimationData* mAnimData;
	int mCurrentFrame;
	
	int mFPS;
	double mTimePerFrame;
	double mTimer;

};
