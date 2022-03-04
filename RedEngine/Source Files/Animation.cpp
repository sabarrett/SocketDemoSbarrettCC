#include "Animation.h"
#include "Sprite.h"
#include "GraphicsBuffer.h"
#include "AnimationData.h"

Animation::Animation(AnimationData* data, int fps)
{
	mAnimData = data;

	mCurrentFrame = 0;

	mFPS = fps;
	mTimePerFrame = 1.0 / fps;
	mTimer = 0.0;
}

Animation::~Animation()
{
	
}

void Animation::update(double deltaTime)
{
	mTimer += deltaTime;
	while (mTimer >= mTimePerFrame)
	{
		mTimer -= mTimePerFrame;
		
		mCurrentFrame++;
		if (mCurrentFrame >= mAnimData->getFrameCount())
		{
			mCurrentFrame = 0;
		}
	}
}

Sprite* Animation::getCurrentSprite()
{
	return mAnimData->getSprite(mCurrentFrame);
}