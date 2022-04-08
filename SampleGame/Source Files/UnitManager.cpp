#include "UnitManager.h"
#include "Unit.h"
#include "Animation.h"
#include "GraphicsSystem.h"
#include "Sprite.h"

using namespace std;

UnitManager::UnitManager()
{
    nextID = 0;
}

UnitManager::~UnitManager()
{
    if(!mUnits.empty())
    {
        clear();
    }
}

Unit* UnitManager::createUnit(Animation* anim, Vector2D loc, Vector2D moveDir, float speed)
{
    return new Unit(anim, loc, moveDir, speed);
}

Unit* UnitManager::createAndManageUnit(Animation* anim, int ID, Vector2D loc, Vector2D moveDir, float speed)
{
    Unit* unit = new Unit(anim, loc, moveDir, speed);
    mUnits.push_back(unit);

    if (ID == 0)
    {
        mIDs.push_back(nextID);
        nextID++;
    }
    else
    {
        mIDs.push_back(ID);
        if (ID >= nextID)
            nextID = ID + 1;
    }
    
    
    return unit;
}

void UnitManager::addUnit(Unit* unit)
{
    mUnits.push_back(unit);
    mIDs.push_back(nextID);
    nextID++;
}

void UnitManager::removeUnit(Unit* unit)
{
    vector<int>::iterator j = mIDs.begin();
    for(vector<Unit*>::iterator i = mUnits.begin(); i != mUnits.end(); i++, j++)
    {
        if(*i == unit)
        {
            mUnits.erase(i);
            mIDs.erase(j);
            return;
        }
    }
}

void UnitManager::removeAndDeleteUnit(Unit* unit)
{
    vector<int>::iterator j = mIDs.begin();
    for(vector<Unit*>::iterator i = mUnits.begin(); i != mUnits.end(); i++, j++)
    {
        if(*i == unit)
        {
            delete *i;
            mUnits.erase(i);
            mIDs.erase(j);
            return;
        }
    }
}

void UnitManager::deleteUnit(Unit* unit)
{
    delete unit;
}

int UnitManager::find(Unit* unit)
{
    int index = 0;
    for(vector<Unit*>::iterator i = mUnits.begin(); i != mUnits.end(); i++)
    {
        if(*i == unit)
        {
            return index;
        }
        index++;
    }
    return -1;
}

Unit* UnitManager::getUnitAt(int index)
{
    int ind = 0;
    for(vector<Unit*>::iterator i = mUnits.begin(); i != mUnits.end(); i++)
    {
        if(ind != index)
        {
            ind++;
            continue;
        }

        return *i;
    }
    return nullptr;
}

int UnitManager::getIDAt(int index)
{
    int ind = 0;
    for (vector<int>::iterator i = mIDs.begin(); i != mIDs.end(); i++)
    {
        if (ind != index)
        {
            ind++;
            continue;
        }

        return *i;
    }
    return -2;
}

Unit* UnitManager::getUnitAtID(int index)
{
    vector<int>::iterator j = mIDs.begin();
    for (vector<Unit*>::iterator i = mUnits.begin(); i != mUnits.end(); i++, j++)
    {
        if (*j == index)
        {
            return *i;
        }
        
    }
    return nullptr;
}

Unit* UnitManager::getUnitAtLoc(Vector2D loc)
{
    for (vector<Unit*>::iterator i = mUnits.begin(); i != mUnits.end(); i++)
    {
        Unit* u = *i;

        if (loc.getX() > u->mLoc.getX() - u->mAnimation->getCurrentSprite()->getSize().getX() / 2.0f
            && loc.getX() < u->mLoc.getX() + u->mAnimation->getCurrentSprite()->getSize().getX() / 2.0f
            && loc.getY() > u->mLoc.getY() - u->mAnimation->getCurrentSprite()->getSize().getY() / 2.0f
            && loc.getY() < u->mLoc.getY() + u->mAnimation->getCurrentSprite()->getSize().getY() / 2.0f)
            return u;
    }
    return nullptr;
}

void UnitManager::draw(GraphicsSystem* gs)
{
    for(vector<Unit*>::iterator i = mUnits.begin(); i != mUnits.end(); i++)
    {
        (*i)->draw(gs);
    }
}

void UnitManager::update(float deltaTime)
{
    for(vector<Unit*>::iterator i = mUnits.begin(); i != mUnits.end(); i++)
    {
        (*i)->update(deltaTime);
    }
}

void UnitManager::clear()
{

    if(mUnits.size() > 0)
    {
        for(vector<Unit*>::iterator i = mUnits.begin(); i != mUnits.end(); i++)
        {
            delete (*i);
        }
    }

    mUnits.clear();

}