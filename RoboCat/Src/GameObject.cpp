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

void GameObject::UpdatePosition()
{
	int val;
	val = rand() % 4;

	if (val == 0)
	{
		//have it move up and to the right
		positionX += 20;
		positionY += 20;
	}

	if (val == 1)
	{
		//move down and to the right
		positionX += 20;
		positionY -= 20;
	}

	if (val == 2)
	{
		//down and to the left
		positionX -= 20;
		positionY -= 20;
	}

	if (val == 3)
	{
		//up and to the left
		positionX -= 20;
		positionY += 20;
	}
}

void GameObject::DrawObjects()
{
	if (objectType == 'r')
	{
		al_draw_filled_circle(positionX, positionY, 10, al_map_rgb(255, 0, 0));
	}

	if (objectType == 'g')
	{
		al_draw_filled_circle(positionX, positionY, 10, al_map_rgb(0, 255, 0));
	}

	if (objectType == 'b')
	{
		al_draw_filled_circle(positionX, positionY, 10, al_map_rgb(0, 0, 255));
	}
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
