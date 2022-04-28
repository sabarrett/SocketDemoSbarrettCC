#pragma once

struct Color
{
	Uint8 r;
	Uint8 g;
	Uint8 b;

	Color()
	{
		r = 255.0;
		g = 255.0;
		b = 255.0;
	}

	Color(Uint8 newR, Uint8 newG, Uint8 newB)
	{
		r = newR;
		g = newG;
		b = newB;
	}
};

