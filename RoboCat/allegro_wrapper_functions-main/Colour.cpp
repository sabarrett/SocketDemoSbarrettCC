/*
Allegro Wrapper Functions
Written by Adel Talhouk in FA21
Colour.cpp
*/

#include "Colour.h"

//Constructor - without alpha
Colour::Colour(unsigned __int8 r, unsigned __int8 g, unsigned __int8 b)
{
	mR = r;
	mG = g;
	mB = b;
	mA = 255;
}

//Constructor - with alpha
Colour::Colour(unsigned __int8 r, unsigned __int8 g, unsigned __int8 b, unsigned __int8 a)
{
	mR = r;
	mG = g;
	mB = b;
	mA = a;
}

//Destructor
Colour::~Colour()
{

}