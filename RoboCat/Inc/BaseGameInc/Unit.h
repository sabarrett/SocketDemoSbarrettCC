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
private:
	Animation mMainAnimation;
	Animation mAltAnimation;
	Animation* mpCurrentAnimation;
	Vector2D mPos;

	Unit(const Vector2D& position, const Animation& mainAnimation, const Animation& altAnimation);
	~Unit() {};

};
