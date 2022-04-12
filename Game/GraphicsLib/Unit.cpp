#include "Unit.h"

Unit::Unit()
{
	//Default constructor
	Animation anim = Animation();
	mAnimations = vector<Animation>(2, anim);
	mX = 0;
	mY = 0;
	mCurrentAnim = 0;
	mUnitID = rand() % 100001;
}

Unit::Unit(int x, int y)
{
	//Filled constructor
	Animation anim = Animation();
	mAnimations = vector<Animation>(2, anim);
	mX = x;
	mY = y;
	mCurrentAnim = 0;
	mUnitID = rand() % 100001;
}

Unit::Unit(int x, int y, Sprite newsprite)
{
	//Filled constructor
	Animation anim = Animation();
	anim.addSprite(newsprite);
	mAnimations = vector<Animation>(2, anim);
	mX = x;
	mY = y;
	mCurrentAnim = 0;
	mUnitID = rand() % 100001;
}

Unit::Unit(int x, int y, Sprite newsprite, int type)
{
	//Filled constructor
	Animation anim = Animation();
	anim.addSprite(newsprite);
	mAnimations = vector<Animation>(2, anim);
	mX = x;
	mY = y;
	mType = type;
	mCurrentAnim = 0;
	mUnitID = rand() % 100001;
}

Unit::Unit(int x, int y, Sprite newsprite, int type, int uID)
{
	//Filled constructor
	Animation anim = Animation();
	anim.addSprite(newsprite);
	mAnimations = vector<Animation>(2, anim);
	mX = x;
	mY = y;
	mType = type;
	mCurrentAnim = 0;
	mUnitID = uID;
}

Unit::Unit(int x, int y, Animation anim1, Animation anim2)
{
	Animation anim = Animation();
	mAnimations.push_back(anim1);
	mAnimations.push_back(anim2);
	mX = x;
	mY = y;
	mCurrentAnim = 0;
	mUnitID = rand() % 100001;
}

Unit::~Unit()
{
}

void Unit::update(double deltaTime)
{
	mAnimations.at(mCurrentAnim).update(deltaTime);
}

void Unit::draw(GraphicsSystem * gSystem)
{
	//draw to the graphics system
	gSystem->draw(mX, mY, mAnimations.at(mCurrentAnim).getCurrentSprite(), 1);
}

void Unit::setAnimation(int newAnimation)
{
	mCurrentAnim = newAnimation;
}

void Unit::movePos(int xVel, int yVel)
{
	mX -= xVel;
	mY -= yVel;
}

int Unit::getAnimation()
{
	return mCurrentAnim;
}

void Unit::addAnimation(Animation anim)
{
}

