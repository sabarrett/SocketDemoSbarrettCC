#pragma once
#include <iostream>
#include <string>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "../common/DeanLib/include/Trackable.h"


const std::string FONT_ASSET_PATH = "..\\..\\common\\assets\\cour.ttf";
const int FONT_SIZE = 48;

class Font : public Trackable
{
public:
	Font(int size = FONT_SIZE);
	~Font();

	void setSize(int size); // This will also destroy and reinitialize the mFont so that size changes are actually reflected.
	ALLEGRO_FONT* getFont() { return mpFont; }

	int getSize() {return mFontSize; }

private:
	ALLEGRO_FONT* mpFont;
	int mFontSize;
};