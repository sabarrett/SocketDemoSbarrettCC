#pragma once

using namespace std;

#include <vector>

#include "Animation.h"
#include "Trackable.h"
#include "GraphicsSystem.h"

class Unit : public Trackable
{
private:
	//current state of unit

	//All animations
	//Sprite sprite;
	vector<Animation> mAnimations;

	Unit();
	Unit(int x, int y);
	Unit(int x, int y, Sprite newsprite);
	Unit(int x, int y, Sprite newsprite, int type);
	Unit(int x, int y, Sprite newsprite, int type, int uID);
	Unit(int x, int y, Animation anim1, Animation anim2);
	~Unit();

	//State changes
	void update(double deltaTime);
	void draw(GraphicsSystem* gSystem);
	void setAnimation(int newAnimation);

	//Get current animation
	int getAnimation();

	void addAnimation(Animation anim);

public:
	int mType;
	int mX, mY = 0;
	int mCurrentAnim;
	int mUnitID;
	friend class UnitManager;
};