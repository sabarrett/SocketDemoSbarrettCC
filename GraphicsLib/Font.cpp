#include "Font.h"

/*
Author: Ryan Littleton, with help from the rest of the class
Class : GPR-250-71
Assignment : Assignment 2
*/

Font::Font(int size)
{
	mFontSize = size;
	mpFont = al_load_ttf_font((FONT_ASSET_PATH).c_str(), mFontSize, 0);
}

Font::~Font()
{
}

void Font::setSize(int size) 
{
	al_destroy_font(mpFont);
	mFontSize = size;
	mpFont = al_load_font((FONT_ASSET_PATH).c_str(), mFontSize, 0);
}
