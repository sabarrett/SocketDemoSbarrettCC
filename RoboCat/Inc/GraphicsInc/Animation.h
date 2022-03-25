#pragma once

#include "Sprite.h"
#include <vector>

class GraphicsBuffer;

class Animation
{
public:
	Animation(float timePerFrame, bool isPaused = false, bool looping = true);
	Animation(const GraphicsBuffer& buffer, 
		int spriteW, 
		int spriteH, 
		int numAcross, 
		int numDown,
		float timePerFrame,
		bool mIsPaused = false,
		bool looping = true);
	~Animation() {};

	void addSprite(const Sprite& theSprite);
	void setTimePerFrame(float newTimePerFrame);
	float getTimePerFrame() const { return mTimePerFrame; };

	void setPauseState(bool shouldPause) { mIsPaused = shouldPause; };
	void togglePause() { mIsPaused = !mIsPaused; };

	void synch(const Animation& otherAnimation);

	const Sprite& getCurrentSprite() const;
	void update(double dt);

private:
	std::vector<Sprite> mSprites;
	int mCurrentSprite;
	bool mLooping;
	bool mIsPaused;
	float mTimePerFrame;
	float mTimeUntilNextFrame;
};
