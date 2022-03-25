#pragma once

#include "Colour.h"
#include "InputSystem.h"
#include "GraphicsLibrary.h"
#include <vector>

//Some functions to make shapes for the game objects to track and send info about
//al_draw_circle
//al_draw_triangle
//al_draw_rectangle

class GameObject
{
	private:
		Colour mColor = Colour(0, 255, 0);
		int objectCount;

		std::vector<int> xPosition; //unsure if there is a way to track both in the same vector
		std::vector<int> yPosition;
	public:
		GameObject();
		~GameObject();

		void CreateObject();
		void DeleteObject();
};