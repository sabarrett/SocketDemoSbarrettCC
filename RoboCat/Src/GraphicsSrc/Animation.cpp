#include "Animation.h"
#include <cassert>
#include <iostream>


Animation::Animation(float timePerFrame, bool isPaused /*= false*/, bool looping /*= true*/)
	:mCurrentSprite(0)
	,mLooping(looping)
	,mIsPaused(isPaused)
	,mTimePerFrame(timePerFrame)
	,mTimeUntilNextFrame(timePerFrame)
{
}

Animation::Animation(const GraphicsBuffer& buffer, int spriteW, int spriteH, int numAcross, int numDown, float timePerFrame, bool isPaused /*= false*/, bool looping /*= true*/)
	:Animation(timePerFrame, isPaused, looping)
{
	for (int y = 0; y < numDown; y++)
	{
		for (int x = 0; x < numAcross; x++)
		{
			Sprite sprite(&buffer, Vector2D((int)(x*spriteW), (int)(y*spriteH)), spriteW, spriteH);
			mSprites.push_back(sprite);
		}
	}
}

void Animation::addSprite(const Sprite& theSprite)
{
	mSprites.push_back(theSprite);
}

void Animation::setTimePerFrame(float newTimePerFrame)
{
	mTimePerFrame = newTimePerFrame;
	if (mTimeUntilNextFrame > mTimePerFrame)
	{
		mTimeUntilNextFrame = mTimePerFrame;
	}
	//std::cout << mTimePerFrame << std::endl;
}

//in order to be synched animations must have the same number of frames
void Animation::synch(const Animation& otherAnimation)
{
	if (mSprites.size() == otherAnimation.mSprites.size())
	{
		mCurrentSprite = otherAnimation.mCurrentSprite;
		mTimePerFrame = otherAnimation.mTimePerFrame;
		mTimeUntilNextFrame = otherAnimation.mTimeUntilNextFrame;
		mIsPaused = otherAnimation.mIsPaused;
	}
}

const Sprite& Animation::getCurrentSprite() const
{
	assert(mCurrentSprite < (int)mSprites.size());
	return mSprites[mCurrentSprite];
}

void Animation::update(double dt)
{
	if (mIsPaused)//paused - nothing to do
	{
		return;//do nothing
	}

	//non-looping animation at the end?
	if (!mLooping && mCurrentSprite == mSprites.size() - 1)
	{
		return;//do nothing
	}

	mTimeUntilNextFrame -= (float)dt;

	if (mTimeUntilNextFrame <= 0.0f)
	{
		mCurrentSprite++;
		mCurrentSprite %= mSprites.size();//makes it loop
		mTimeUntilNextFrame = mTimePerFrame;
	}
}

