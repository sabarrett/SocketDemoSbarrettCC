#pragma once

#include "Colour.h"

//Some functions to make shapes for the game objects to track and send info about
//al_draw_circle
//al_draw_triangle
//al_draw_rectangle

class GameObject
{
	private:
		Colour mColor = Colour(0, 255, 0);
		//i need something to track the objects probably
	public:
		GameObject();
		~GameObject();

		void CreateObject(int val);
		void DeleteObject();
};