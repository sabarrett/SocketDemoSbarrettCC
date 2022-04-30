#include "RoboCatPCH.h"
#include "UnitManager.h"

void UnitManager::createReceivedUnit(Vector2 _pos, Vector2 _size, Color _col, int _type, int _id)
{
	Unit temp = Unit();
	bool canCreate = true;
	switch (_type)
	{
	case 0: // square
	case 1: // vRect
	case 2: // hRect
		temp = Unit(Vector2(_pos.x, _pos.y), _size, _type, _col, _id);
		break;
	default:
		canCreate = false;
	}

	if (canCreate)
	{
		units.push_back(temp);
		count++;
	}
}

int UnitManager::createSquare(Vector2 screen)
{
	int posx = GetRandInt(0, screen.x);
	int posy = GetRandInt(0, screen.y);
	int size = GetRandInt(20, 121)    ;
	Color col = colors.at(GetRandInt(0, colors.size()));

	Unit temp = Unit(Vector2(posx, posy), Vector2(size, size), 0, col, GetRandUnitInt(0,1000));
	units.push_back(temp);
	count++;
	return temp.getID();
}

int UnitManager::createRectV(Vector2 screen)
{
	int posx = GetRandInt(0, screen.x);
	int posy = GetRandInt(0, screen.y);
	int sizex = GetRandInt(20, 40);
	int sizey = GetRandInt(40, 61);
	Color col = colors.at(GetRandInt(0, colors.size()));

	Unit temp = Unit(Vector2(posx, posy), Vector2(sizex, sizey), 1, col, GetRandUnitInt(0, 1000));
	units.push_back(temp);
	count++;
	return temp.getID();
}

int UnitManager::createRectH(Vector2 screen)
{
	int posx = GetRandInt(0, screen.x);
	int posy = GetRandInt(0, screen.y);
	int sizex = GetRandInt(60, 81);
	int sizey = GetRandInt(40, 60);
	Color col = colors.at(GetRandInt(0, colors.size()));

	Unit temp = Unit(Vector2(posx, posy), Vector2(sizex, sizey), 2, col, GetRandUnitInt(0, 1000));
	units.push_back(temp);
	count++;
	return temp.getID();
}

void UnitManager::createColors()
{
	colors.push_back(Color(200, 75, 60, 255));
	colors.push_back(Color(61, 226, 255, 255));
	colors.push_back(Color(155, 132, 245, 255));
	colors.push_back(Color(255, 168, 197, 255));
	colors.push_back(Color(12, 12, 179, 255));
	colors.push_back(Color(252, 0, 93, 255));
	colors.push_back(Color(255, 91, 15, 255));
	colors.push_back(Color(76, 199, 74, 255));
}

void UnitManager::updateUnits(float dt, Vector2 screen)
{
	for (int i = 0; i < count; i++)
		units.at(i).update(dt, screen);
}

void UnitManager::RenderUnits(SDL_Renderer* renderer)
{
	for (int i = 0; i < count; i++)
		units.at(i).render(renderer);
}

int UnitManager::GetRandInt(int min, int max)
{
	int r = std::rand() % max + min;
	return r;
}

int UnitManager::GetRandUnitInt(int min, int max)
{
	int r = std::rand() % max + min;
	bool validID = true;
	for (int i = 0; i < count; i++)
		if (units[i].getID() == r)
			validID = false;

	while (!validID)
	{
		validID = true;
		r = std::rand() % max + min;
		for (int i = 0; i < count; i++)
			if (units[i].getID() == r)
				validID = false;
	}

	return r;
}

Unit* UnitManager::getUnit(int id)
{
	Unit* toReturn = nullptr;

	for (int i = 0; i < count; i++)
		if (units[i].getID() == id)
		{
			toReturn = &units[i];
			break;
		}

	return toReturn;
}

int UnitManager::getUnitIteratorFromID(int id)
{
	int toReturn = -1;

	for (int i = 0; i < count; i++)
	{
		if (id == units[i].getID())
		{
			toReturn = i;
			break;
		}
	}

	return toReturn;
}

Unit* UnitManager::getRandomUnit()
{
	Unit* toReturn = nullptr;

	if (count > 0)
		toReturn = &units[rand() % count];

	return toReturn;
}

void UnitManager::deleteUnit(int id)
{
	int it = getUnitIteratorFromID(id);
	if (it != -1)
		units.erase(units.begin() + it);
	count--;
}
