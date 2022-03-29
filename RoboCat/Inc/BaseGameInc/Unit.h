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

	Unit(const Vector2D& position, const Animation& mainAnimation, const Animation& altAnimation);
	Unit() { mMainAnimation = NULL; mAltAnimation = NULL; mpCurrentAnimation = nullptr; };
	virtual ~Unit() {};

	virtual void update(double dt);
	virtual void draw();
	virtual void setPosition(const Vector2D& newPos) { mPos = newPos; };
	virtual void toggleAnimation();
	void speedUpAnimation();
	void slowDownAnimation();
	virtual void setAnimationPauseState(bool shouldPause);
	virtual void toggleAnimationPauseState();
	virtual bool doesPointIntersect(const Vector2D& point) const;
	virtual Vector2D getULPosition() const;
	virtual Vector2D getCenterPosition() const { return mPos; };
	virtual Vector2D getLRPosition() const;

private:
	Animation mMainAnimation;
	Animation mAltAnimation;
	Animation* mpCurrentAnimation;
	Vector2D mPos;
};

class RandomDirUnit : public Unit
{
public:
	RandomDirUnit(const Vector2D& position, Sprite& sprite);

	virtual ~RandomDirUnit() {};

	virtual void update(double dt);
	virtual void draw();
	virtual void setPosition(const Vector2D& newPos) { mPos = newPos; };
	
	Vector2D getCenterPosition() const { return mPos; };

private:
	Animation mMainAnimation;
	Animation mAltAnimation;
	Animation* mpCurrentAnimation;
	Vector2D mPos;
	Sprite& mSprite;
	//seed for rand

};

class RandomSpawnedUnit : public Unit
{
public:
	RandomSpawnedUnit(const Vector2D& position, const Animation& mainAnimation, const Animation& altAnimation, int seed);

	virtual ~RandomSpawnedUnit() {};

	virtual void update(double dt);
	virtual void draw();
	virtual void setPosition(const Vector2D& newPos) { mPos = newPos; };
	virtual void toggleAnimation();
	virtual bool doesPointIntersect(const Vector2D& point) const;
	virtual Vector2D getULPosition() const;
	virtual Vector2D getCenterPosition() const { return mPos; };
	virtual Vector2D getLRPosition() const;
	virtual void setAnimationPauseState(bool shouldPause);
	virtual void toggleAnimationPauseState();

private:
	Animation mMainAnimation;
	Animation mAltAnimation;
	Animation* mpCurrentAnimation;
	Vector2D mPos;
	int mSeed;
};
