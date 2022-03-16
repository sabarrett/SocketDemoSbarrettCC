#pragma once
#include <iostream>
#include <string>
#include <allegro5/allegro.h>
#include "../common/DeanLib/include/Trackable.h"

class Color : public Trackable
{
public:
	// Constructor. Will create a fully opaque white color if no values are input.
	Color(int r = 0, int g = 0, int b = 0, int alpha = 255);

	// Returns the stored RGB values as an ALLEGRO_COLOR
	ALLEGRO_COLOR getColor();

private: // Data here stores the RGBA values from constructor as ints. I opted to use these instead of an ALLEGRO_COLOR so that if someone wants to change it in the future all they need to do is change the getColor function.
	int mR;
	int mG;
	int mB;
	int mAlpha;
};