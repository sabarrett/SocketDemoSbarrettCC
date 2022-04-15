#include "Color.h"

/*
Author: Ryan Littleton, with help from the rest of the class
Class : GPR-250-71
Assignment : Assignment 2
*/

Color::Color(int r, int g, int b, int alpha) 
{
	mR = r;
	mG = g;
	mB = b;
	mAlpha = alpha;
}

ALLEGRO_COLOR Color::getColor() {
	return al_map_rgba(mR, mG, mB, mAlpha);
}
