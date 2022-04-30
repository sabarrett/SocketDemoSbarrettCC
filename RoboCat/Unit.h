#pragma once
#include <RoboCatPCH.h>
#include "Color.h"
#include "SDL.h"
#include <ostream>

struct Vector2
{
	int x;
	int y;

	Vector2::Vector2() { x = 0; y = 0; };
	Vector2::Vector2(float a, float b) { x = a; y = b; };

	friend std::ostream& operator<<(std::ostream& out, const Vector2& vec)
	{
		out << "[" << vec.x << "," << vec.y << "]";
		return out;
	}
	
	std::string ToString()
	{
		return "[" + std::to_string(x) + "," + std::to_string(y) + "]";
	}
};

class Unit
{
private:

	Vector2 position;
	Vector2 size; // width + height
	Color color;

	SDL_Rect rect;

	// 0 - square, 1 - vRect, 2 - hRect
	int type;
	int unitID;

	bool right;

public:

	Unit::Unit()
	{
		position = Vector2(0, 0);
		size = Vector2(1, 1);
		type = 0;

		initRect();
		right = true;
	}

	Unit::Unit(Vector2 pos, Vector2 s, int t, Color col, int id)
	{
		position = pos;
		size = s;
		type = t;
		color = col;
		unitID = id;

		initRect();
		right = true;
	}

	void initRect()
	{
		rect.x = position.x;
		rect.y = position.y;
		rect.w = size.x;
		rect.h = size.y;
	}

	Vector2 getPosition() { return position; };
	void setPosition(Vector2 pos) { position = pos; rect.x = position.x; rect.y = position.y; };
	int getType() { return type; };
	SDL_Rect getRect() { return rect; };

	void update(float dt, Vector2 screen);

	void render(SDL_Renderer* renderer);

	int getID() { return unitID; };
	Vector2 getSize() { return size; };
	Color getColor() { return color; };
};