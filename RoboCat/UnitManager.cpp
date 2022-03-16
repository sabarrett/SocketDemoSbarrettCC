#include"UnitManager.h"
#include"Unit.h"
#include"Animation.h"
#include"Sprite.h"
#include"System.h"

UnitManager::UnitManager()
{
	if (mpUnits != nullptr)
	{
		cleanup();
	}
	else
	{
		mpUnits = new vector<Unit*>();
	}
}

UnitManager::~UnitManager()
{
	if (mpUnits != nullptr)
	{
		cleanup();
	}
}

void UnitManager::addUnits(Unit* units)
{
	mpUnits->push_back(units);
}

void UnitManager::deleteUnits(int deletedUnits)
{
	delete mpUnits->at(deletedUnits);

	mpUnits->erase(mpUnits->begin() + deletedUnits);
}

Unit* UnitManager::getUnits(int getUnits)
{
	return mpUnits->at(getUnits);
}

void UnitManager::clearUnits()
{
	cleanup();
}

void UnitManager::updateUnits(float elapsedTime)
{
	int i = 0;

	for (i; i < mpUnits->size(); i++)
	{
		mpUnits->at(i)->locationUpdate(elapsedTime);
	}
}

void UnitManager::draw(System* system)
{
	for (int i = 0; i < mpUnits->size(); i++)
	{
		mpUnits->at(i)->drawSprite(system);
	}
}

int UnitManager::getNumUnits()
{
	if (mpUnits != nullptr)
	{
		return mpUnits->size();
	}
	else
	{
		return 0;
	}
}

void UnitManager::cleanup()
{
	for (int i = 0; i < mpUnits->size(); i++)
	{
		delete mpUnits->at(i);
	}

	mpUnits->clear();
}