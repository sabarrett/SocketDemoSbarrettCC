#include "Animation.h"

Animation::Animation()
{
	mUntilFrame = 0;
	mLoop = false;
	mCurrentSprite = 0;
	mFrameTiming = 0;
}

Animation::Animation(double time, bool loop)
{
	mUntilFrame = 0;
	mFrameTiming = time;
	mLoop = loop;
	mCurrentSprite = 0;
}

Animation::~Animation()
{
}

void Animation::addSprite(Sprite newSprite)
{
	mSpriteList.push_back(newSprite);
}

void Animation::deleteSprite(int pos)
{
	mSpriteList.erase(mSpriteList.begin() + pos);
}

void Animation::update(double deltaTime)
{
	if (!paused)
	{
		//update current frames time
		mUntilFrame -= deltaTime;

		//check if need to update sprite
		if (mUntilFrame <= 0)
		{
			//go forward one sprite
			mCurrentSprite++;

			//check if need to loop or not
			if ((int16_t)mCurrentSprite >= (int16_t)mSpriteList.size() - 1)
			{
				if (mLoop)
				{
					mCurrentSprite = 0;
				}
				else
				{
					mCurrentSprite = mSpriteList.size() - 1;
				}
			}

			//reset frame time
			mUntilFrame = mFrameTiming;
		}
	}
}

Sprite Animation::getCurrentSprite()
{
	return mSpriteList.at(mCurrentSprite);
}

void Animation::speedUp(int modify)
{
	mFrameTiming -= modify; //update timing between frames

	//cap animation at -2000ms between frames
	if (mFrameTiming < -2000)
	{
		mFrameTiming = -2000;
	}
}

void Animation::slowDown(int modify)
{
	mFrameTiming += modify; //update timing between frames

	//cap animation at 2000ms between frames
	if (mFrameTiming > 2000)
	{
		mFrameTiming = 2000;
	}
}

void Animation::setTiming(double time)
{
	mFrameTiming = time;
}

void Animation::setLooping(bool loop)
{
	mLoop = loop;
}

void Animation::setPaused()
{
	paused = !paused;
}
