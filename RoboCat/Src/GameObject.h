#pragma once

#include "Colour.h"
#include "InputSystem.h"
#include "GraphicsLibrary.h"
#include <vector>

//do i want this function to work like this as a sort of manager?
//my alternative thought is that the array is in main, and here is just a single object
//then just pick one to delete in main and delete it here or something

class GameObject
{
	private:
		Colour mColor = Colour(0, 255, 0);
		int objectCount;
		int positionX; //floats for positions?
		int positionY;
	public:
		GameObject();
		~GameObject();

		void CreateObject();
		void DeleteObject();
};