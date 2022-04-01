#pragma once

/*
Allegro Wrapper Functions
Written by Adel Talhouk in FA21
Colour.h
	File information:
	This file contains data used for colours.
*/

class Color
{
	//-------------------------Private data-------------------------

	//Red channel
	unsigned __int8 mR;

	//Green channel
	unsigned __int8 mG;

	//Blue channel
	unsigned __int8 mB;

	//Alpha channel
	unsigned __int8 mA;

	//-------------------------Public data-------------------------
public:

	//Constructor(s)
	Color(unsigned __int8 r, unsigned __int8 g, unsigned __int8 b);
	Color(unsigned __int8 r, unsigned __int8 g, unsigned __int8 b, unsigned __int8 a);

	//Destructor
	~Color();

	//Accessor(s)
	unsigned __int8 getR() { return mR; };
	unsigned __int8 getG() { return mG; };
	unsigned __int8 getB() { return mB; };
	unsigned __int8 getA() { return mA; };
};