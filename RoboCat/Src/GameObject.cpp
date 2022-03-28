#include "GameObject.h"

GameObject::GameObject()
{
	mColor = Colour(0, 255, 0);
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
		al_draw_filled_circle(400, 400, 10, al_map_rgb(0, 255, 0));
		objectType = 'g';
		positionX = 400;
		positionY = 400;
	}
	else if (val == 1)
	{
		al_draw_filled_circle(400, 400, 10, al_map_rgb(255, 0, 0));
		objectType = 'r'; 
		positionX = 400;
		positionY = 400;

		//al_draw_triangle();
	}
	else if (val == 2)
	{
		al_draw_filled_circle(400, 400, 10, al_map_rgb(0, 0, 255));
		objectType = 'b';
		positionX = 400;
		positionY = 400;

		//al_draw_rectangle();
	}

	//probably no need for a final else statement
}

void GameObject::DeleteObject()
{
	//idk if there is any deletion i need to do necessarily
		//possibly just undrawing things
	
}

char GameObject::GetObjectType()
{
	return objectType;
}

float GameObject::GetPositionX()
{
	return positionX;
}

float GameObject::GetPositionY()
{
	return positionY;
}
