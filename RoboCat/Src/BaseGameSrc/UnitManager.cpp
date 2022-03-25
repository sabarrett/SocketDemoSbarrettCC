#include "UnitManager.h"
#include "Unit.h"
#include "Game.h"

#include <Animation.h>
#include <cassert>
#include "MemoryManager.h"

using namespace std;

UnitManager::UnitManager()
{
}

UnitManager::~UnitManager()
{
	clear();
}

void UnitManager::clear()
{
	/*vector<Unit*>::iterator iter;

	for (iter = mUnits.begin(); iter != mUnits.end(); ++iter)
	{
	}*/
	MemoryManager* pMemoryManager = Game::getInstance()->getMemoryManager();
	for (auto iter : mUnits)
	{
		
		iter->~Unit();
		pMemoryManager->deallocate((Byte*)iter);
	}

	mUnits.clear();
}

void UnitManager::update(double dt)
{
	for (unsigned int i = 0; i < mUnits.size(); i++)
	{
		mUnits[i]->update(dt);
	}
}

void UnitManager::draw() const
{
	vector<Unit*>::const_iterator iter;
	for (iter = mUnits.begin(); iter != mUnits.end(); ++iter)
	{
		(*iter)->draw();
	}
}

Unit* UnitManager::createUnit(const Vector2D& position, const Animation& mainAnimation, const Animation& altAnimation)
{
	MemoryManager* pMemoryManager = Game::getInstance()->getMemoryManager();

	Byte* ptr = pMemoryManager->allocate(sizeof(Unit));

	Unit* pUnit = new(ptr)Unit(position, mainAnimation, altAnimation);//placement new create the unit.



	mUnits.push_back(pUnit);//put the unit into the vector

	return pUnit;//return the newly created Unit (for convenience)
}

void UnitManager::deleteAllUnitsAt2DPosition(const Vector2D& position)
{
	while (deleteUnitAt2DPosition(position));
}

bool UnitManager::deleteUnitAt2DPosition(const Vector2D& position)
{
	for (unsigned int i = 0; i < mUnits.size(); i++)
	{
		if (mUnits[i]->doesPointIntersect(position))
		{
			deleteUnitAtIndex(i);//only delete one unit
			return true;
		}
	}
	return false;
}

void UnitManager::deleteUnitAtIndex(unsigned int index)
{
	assert(index >= 0 && index < mUnits.size());
	MemoryManager* pMemoryManager = Game::getInstance()->getMemoryManager();
	if (index >= 0 && index < mUnits.size())
	{
		mUnits[index]->~Unit();
		pMemoryManager->deallocate((Byte*)mUnits[index]);
		mUnits.erase(mUnits.begin() + index);

		
	}

}

void UnitManager::deleteUnit(Unit* pUnit)
{
	vector<Unit*>::iterator iter;
	MemoryManager* pMemoryManager = Game::getInstance()->getMemoryManager();
	for (iter = mUnits.begin(); iter != mUnits.end(); ++iter)
	{
		if (*iter == pUnit)
		{
			pUnit->~Unit(); //Placement new destructor call. Need to do it.
			pMemoryManager->deallocate((Byte*)pUnit);
			mUnits.erase(iter);
			break;
		}
	}
}

Unit* UnitManager::getLastUnitCreated() const
{
	if (mUnits.size() > 0)
	{
		return mUnits[mUnits.size() - 1];
	}
	else
		return NULL;
}

void UnitManager::setPauseStateForAllAnimations(bool isPaused)
{
	for (unsigned int i = 0; i < mUnits.size(); i++)
	{
		mUnits[i]->setAnimationPauseState(isPaused);
	}
}

void UnitManager::togglePauseStateForAllAnimations()
{
	for (unsigned int i = 0; i < mUnits.size(); i++)
	{
		mUnits[i]->toggleAnimationPauseState();
	}
}
