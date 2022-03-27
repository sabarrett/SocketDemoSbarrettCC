#include "GameObject.h"

GameObject::GameObject()
{
	mColor = Colour(0, 255, 0);
	objectCount = 0;
}

GameObject::~GameObject()
{

}

void GameObject::CreateObject()
{
	int val;
	val = rand() % 3;

	if (val == 0)
	{
		//al_draw_filled_circle();
	}
	else if (val == 1)
	{
		//al_draw_triangle();
	}
	else if (val == 2)
	{
		//al_draw_rectangle();
	}
	
	objectCount++;
	//i dont think i need another else statement but maybe could have one just in case
}

void GameObject::DeleteObject()
{
	int tmpRand;
	tmpRand = rand() % objectCount;

	//iterate through the list of positions tmp rand times
	//delete the object that it lands on

	objectCount--;
}
