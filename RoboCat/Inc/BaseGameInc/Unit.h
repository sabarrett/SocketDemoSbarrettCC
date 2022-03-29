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
	~Unit() {};

	virtual void update(double dt);
	virtual void draw();
	virtual void setPosition(const Vector2D& newPos) { mPos = newPos; };
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

	

};

class RandomDirUnit : public Unit
{
public:
	RandomDirUnit(const Vector2D& position, Sprite& sprite);

	~RandomDirUnit() {};

	virtual void update(double dt);
	virtual void draw();
	virtual void setPosition(const Vector2D& newPos) { mPos = newPos; };
	
	Vector2D getCenterPosition() const { return mPos; };

	uint32_t getNetworkID() { return mNetworkID; }
	void setNetworkID(uint32_t id) { mNetworkID = id; };
private:
	Vector2D mPos;
	Sprite& mSprite;
	//seed for rand

	uint32_t mNetworkID;
};

class RandomSpawnedUnit : public Unit
{
public:
	RandomSpawnedUnit(const Vector2D& position, Sprite& sprite);

	~RandomSpawnedUnit() {};

	virtual void update(double dt);
	virtual void draw();
	virtual void setPosition(const Vector2D& newPos) { mPos = newPos; };

	Vector2D getCenterPosition() const { return mPos; };

	uint32_t getNetworkID() { return mNetworkID; }
	void setNetworkID(uint32_t id) { mNetworkID = id; };
private:
	Vector2D mPos;
	Sprite& mSprite;
	uint32_t mNetworkID;
};
