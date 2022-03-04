#include "Unit.h"
#include "GraphicsSystem.h"
#include "Animation.h"
#include "Sprite.h"

Unit::Unit()
{
	mAnimation = nullptr;
}

Unit::Unit(Animation* anim, Vector2D loc, Vector2D moveDir, float speed)
{
	mAnimation = anim;
	mLoc = loc;
	mMoveDir = moveDir.normalized();
	mSpeed = speed;
}

Unit::~Unit()
{

}

void Unit::draw(GraphicsSystem* gs)
{
	
	gs->draw(mAnimation->getCurrentSprite(), mLoc - (mAnimation->getCurrentSprite()->getSize() * mAnimation->getCurrentSprite()->getScale()) / 2.0f); //Draw the sprite at the center of the unit location
}

void Unit::update(float deltaTime)
{
	mLoc += mMoveDir * deltaTime * mSpeed;
}