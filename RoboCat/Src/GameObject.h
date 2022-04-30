#pragma once

#include "Colour.h"
#include "InputSystem.h"
#include "GraphicsLibrary.h"
#include <vector>

//I think I will have this be an individual object
	//have most object management be done in main

class GameObject
{
	private:
		Colour mColor = Colour(0, 255, 0);
		float positionX;
		float positionY;
		char objectType;
	public:
		GameObject();
		~GameObject();

		void CreateObject();
		void DeleteObject();
		void UpdatePosition();
		void DrawObjects();

		char GetObjectType();
		float GetPositionX();
		float GetPositionY();
};