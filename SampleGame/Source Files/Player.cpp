#include "Player.h"
#include "EventSystem.h"
#include "PlayerMoveEvent.h"

Player::Player()
    : Unit()
{
    
}

Player::Player(Animation* anim, float moveSpeed, Vector2D loc)
    : Unit(anim, loc)
{
    mMoveDir = Vector2D::Zero();
    mSpeed = moveSpeed;
}

Player::~Player()
{
    
}

void Player::update(double deltaTime)
{

    if(mMoveDir != Vector2D::Zero())
	{
		mLoc += mMoveDir * deltaTime * mSpeed;
		EventSystem::getInstance()->fireEvent(PlayerMoveEvent(mLoc));
	}
}

void Player::setMoveDirection(Vector2D dir)
{
    mMoveDir = dir;
}