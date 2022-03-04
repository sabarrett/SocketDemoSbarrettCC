#pragma once

#include "Trackable.h"
#include "Vector2D.h"

class GraphicsSystem;
class Animation;

class Unit : public Trackable
{
public:
	friend class UnitManager;

	void draw(GraphicsSystem* gs);

	void setLocation(Vector2D loc) { mLoc = loc; }

	void update(float deltaTime);

	Vector2D getLocation() { return mLoc; }

protected:
	Unit();
	Unit(Animation* anim, Vector2D loc = Vector2D::Zero(), Vector2D moveDir = Vector2D::Zero(), float speed = 1.0f);
	~Unit();

	Animation* mAnimation;
	Vector2D mLoc, mMoveDir;
	float mSpeed;

};
