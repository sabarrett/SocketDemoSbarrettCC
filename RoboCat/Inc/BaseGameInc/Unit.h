#pragma once

#include <Trackable.h>
#include <Vector2D.h>
#include <vector>

#include "Animation.h"

class UnitManager;

class Unit : public Trackable
{
	friend class UnitManager;
public:

	void update(double dt);
	void draw();
	void setPosition(const Vector2D& newPos) { mPos = newPos; };
	void toggleAnimation();
	void speedUpAnimation();
	void slowDownAnimation();
	void setAnimationPauseState(bool shouldPause);
	void toggleAnimationPauseState();
	bool doesPointIntersect(const Vector2D& point) const;
	Vector2D getULPosition() const;
	Vector2D getCenterPosition() const { return mPos; };
	Vector2D getLRPosition() const;

	uint32_t getNetworkID() { return mNetworkID; }
	void setNetworkID(uint32_t id) { mNetworkID = id; };
private:
	Animation mMainAnimation;
	Animation mAltAnimation;
	Animation* mpCurrentAnimation;
	Vector2D mPos;

	uint32_t mNetworkID;

	Unit(const Vector2D& position, const Animation& mainAnimation, const Animation& altAnimation);
	~Unit() {};

};
