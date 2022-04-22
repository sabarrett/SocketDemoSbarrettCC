#pragma once

struct Vec2D
{
	double x;
	double y;
	
	Vec2D()
	{
		x = 0.0;
		y = 0.0;
	}

	Vec2D(double newX, double newY)
	{
		x = newX;
		y = newY;
	}

	Vec2D(int newX, int newY)
	{
		x = double(newX);
		y = double(newY);
	}
};