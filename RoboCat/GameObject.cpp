#include "GameObject.h"

GameObject::GameObject()
{
	mColor = Colour(0, 255, 0);
}

GameObject::~GameObject()
{

}

void GameObject::CreateObject(int val)
{
	if (val == 0)
	{
		//al_draw_circle
	}

	if (val == 1)
	{
		//al_draw_triangle
	}

	if (val == 2)
	{
		//al_draw_rectangle
	}
}

void GameObject::DeleteObject()
{

}
