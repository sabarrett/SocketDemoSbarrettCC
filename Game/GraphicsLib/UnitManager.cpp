#include "UnitManager.h"
#include "UnitManager.h"

UnitManager::UnitManager()
{
	mCurrentlyPaused = false;
}

UnitManager::~UnitManager()
{
	//clear();
}

void UnitManager::cleanup()
{
	clear();
}

//ADDERS
void UnitManager::addUnit(Unit* newUnit)
{
	mUnits.push_back(newUnit);
}

void UnitManager::addUnit()
{
	mUnits.push_back(new Unit());
}

void UnitManager::addUnit(int x, int y)
{
	mUnits.push_back(new Unit(x, y));
}

void UnitManager::addUnit(int x, int y, Sprite sprite)
{
	mUnits.push_back(new Unit(x, y, sprite));
}

void UnitManager::addUnit(int x, int y, Sprite sprite, int type)
{
	mUnits.push_back(new Unit(x, y, sprite, type));
}

void UnitManager::addUnit(int x, int y, Animation anim1, Animation anim2)
{
	mUnits.push_back(new Unit(x, y, anim1, anim2));
}


//DELETES
void UnitManager::deleteUnit(int unitLoc)
{
	delete(mUnits[unitLoc]);
	mUnits.erase(mUnits.begin() + unitLoc);
}

void UnitManager::deleteAllUnits()
{
	while (mUnits.size() > 1)
	{
		deleteUnit(1);
	}

	//addUnit(-1000, -1000);
}

void UnitManager::deleteRandomUnit()
{
	if (mUnits.size() > 0)
	{
		deleteUnit(rand() % mUnits.size());
	}
}

void UnitManager::deleteUnit(Unit* delUnit)
{
	int counter = 0;
	for (auto it = mUnits.begin(); it != mUnits.end(); it++)
	{
		if ((*it) == delUnit)
		{
			deleteUnit(counter);
		}

		counter++;
	}
}

void UnitManager::erase(int mouseX, int mouseY, const int UNIT_WIDTH, const int UNIT_HEIGHT)
{
	for (auto it = mUnits.begin(); it != mUnits.end(); it++)
	{
		//check if unit is in range of mouse
		if (((*it)->mX >= mouseX -( UNIT_WIDTH / 2) && (*it)->mX <= mouseX + (UNIT_WIDTH / 2)) && ((*it)->mY >= mouseY - (UNIT_HEIGHT / 2) && (*it)->mY <= mouseY + (UNIT_HEIGHT / 2)))
		{
			if (it != mUnits.begin())
			{
				delete (*it); //delete unit
				mUnits.erase(it--); //erase unit one position behind, which is the current units actual location in the vector?
			}
			else
			{
				delete (*mUnits.begin());
				mUnits.erase(mUnits.begin());
			}
		}
	}
}

//GETTERS
Unit* UnitManager::getUnit(int unitLoc)
{
	return mUnits.at(unitLoc);
}

Unit* UnitManager::getLastUnit()
{
	return mUnits.back();
}

//ANIMATION SETTERS
void UnitManager::setUnitAnim(Unit* unit, int anim)
{
	unit->setAnimation(anim);
}

void UnitManager::setUnitAnim(int unitLoc, int anim)
{
	mUnits.at(unitLoc)->setAnimation(anim);
}

void UnitManager::setUnitAnim(int anim)
{
	for (auto it = mUnits.begin(); it != mUnits.end(); it++)
	{
		(*it)->setAnimation(anim);
	}
}

//ANIMATION ADDERS
void UnitManager::addUnitAnim(Animation anim)
{
	for (auto it = mUnits.begin(); it != mUnits.end(); it++)
	{
		(*it)->addAnimation(anim);
	}
}

void UnitManager::addUnitAnim(Animation anim, Unit* unit)
{
	unit->addAnimation(anim);
}

void UnitManager::addUnitAnim(Animation anim, int unitLoc)
{
	mUnits.at(unitLoc)->addAnimation(anim);
}

//ANIMATION GETTERS
int UnitManager::getUnitAnim(int unitLoc)
{
	return mUnits.at(unitLoc)->getAnimation();
}

int UnitManager::getUnitAnim(Unit* unit)
{
	return unit->getAnimation();
}

//PAUSERS
void UnitManager::pauseAnim()
{
	//run through all units and pause them
	for (auto it = mUnits.begin(); it != mUnits.end(); it++)
	{
		for (auto at = (*it)->mAnimations.begin(); at != (*it)->mAnimations.end(); at++) //do all animations
		{
			(*at).setPaused();
		}
	}
	mCurrentlyPaused = !mCurrentlyPaused;
}

void UnitManager::pauseAnim(int unitLoc)
{
	//pause a unit at location
	for (auto at = mUnits.at(unitLoc)->mAnimations.begin(); at != mUnits.at(unitLoc)->mAnimations.end(); at++) //do all animations
	{
		(*at).setPaused();
	}
}

void UnitManager::pauseAnim(Unit* unit)
{
	//pause a set unit
	for (auto at = unit->mAnimations.begin(); at != unit->mAnimations.end(); at++)  //do all animations
	{
		(*at).setPaused();
	}
}

bool UnitManager::isPaused()
{
	return mCurrentlyPaused;
}

//DELETE ALL UNITS
void UnitManager::clear()
{
	for (auto it = mUnits.begin(); it != mUnits.end(); it++)
	{
		delete (*it);
	}

	mUnits.clear();
}

//UPDATE ALL UNITS
void UnitManager::update(double deltaTime)
{
	//run through all units and call their update function
	for (auto it = mUnits.begin(); it != mUnits.end(); it++)
	{
		(*it)->update(deltaTime);
	}
}

//DRAW ALL UNITS
void UnitManager::draw(GraphicsSystem* gSystem)
{
	//run through all units and call their draw function
	for (auto it = mUnits.begin(); it != mUnits.end(); it++)
	{
		(*it)->draw(gSystem);
	}
}

vector<vector<int>> UnitManager::getAllUnitsLocation()
{
	vector<vector<int>> locs;

	cout << "BIG WHOOP" << endl;

	for (int i = 0; i < mUnits.size(); i++)
	{
		Unit newUnit = *getUnit(i);
		vector<int> loc = vector<int>();
		loc.push_back(newUnit.mType);
		loc.push_back(newUnit.mX);
		loc.push_back(newUnit.mY);
		locs.push_back(loc);
		//loc[i] = Vector2D( getUnit(i)->mX, getUnit(i)->mY );
	}

	for (int i = 0; i < locs.size(); i++)
	{
		cout << "new Loc " << locs[i][0] << " " << locs[i][1] << " " << locs[i][2] << endl;
	}

	return locs;
}

int* UnitManager::getAllUnitsTypes()
{
	int* types = new int[mUnits.size()];
	for (int i = 0; i < mUnits.size(); i++)
	{
		types[i] = getUnit(i)->mType;
	}
	return types;
}

Unit* UnitManager::getAllUnits()
{
	Unit* units = new Unit[mUnits.size()];
	for (int i = 0; i < mUnits.size(); i++)
	{
		units[i] = *getUnit(i);
	}
	return units;
}