#include"Unit.h"
#include"GraphicsLib/GraphicsBuffer.h"
#include"GraphicsLib/GraphicsSystem.h"
#include"GraphicsLib/System.h"
#include"Game.h"

Unit::Unit()
{
	mLocX = 0;
	mLocY = 0;

	mpAnimate = new Animation();
	mIsAnimated = false;
	mScale = 0.0;
}

Unit::Unit(int locX, int locY, Animation* animate)
{
	mLocX = locX;
	mLocY = locY;

	mpAnimate = animate;
	mIsAnimated = true;
	mScale = 1.0;
}

Unit::~Unit()
{
	cleanup();
}

void Unit::init(Animation* animate, int locX, int locY)
{
	mpAnimationList.push_back(animate);
	mpAnimate = mpAnimationList[0];

	mLocX = locX;
	mLocY = locY;
}

void Unit::locationUpdate(float elapsedTime)
{
	mpAnimate->animationUpdate(elapsedTime);
}

void Unit::drawSprite(System* system)
{
	system->getGraphicsSystem()->draw(mLocX, mLocY, mpAnimate->getCurrentSprite(), mScale);
}

void Unit::addAnimation(Animation* toAdd)
{
	mpAnimationList.push_back(toAdd);
}

void Unit::setAnimation(Animation* toAdd)
{
	mpAnimate = toAdd;
}

void Unit::setPosition(int locX, int locY)
{
	mLocX = locX;
	mLocY = locY;
}

void Unit::setAnimationSpeed(int speed)
{
	mpAnimate->changeFPS(speed);
}

void Unit::toggleAnimations(bool animate)
{
	//Iterate through the animations list
	for (auto& p : mpAnimationList)
	{
		//Toggle the animation pause
		p->setAnimate(animate);
	}

	//Toggle the current animation pause
	mpAnimate->setAnimate(animate);
}

void Unit::cleanup()
{
	if (mIsAnimated == true)
	{
		// HACK I think that some this might need to call cleanup on mpAnimate? - wes

		//mpAnimate->cleanup();
		delete mpAnimate;
		mpAnimate = nullptr;

		mIsAnimated = false;
	}
}
