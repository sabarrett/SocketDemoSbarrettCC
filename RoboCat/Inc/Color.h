#pragma once
#include <allegro5/allegro.h>

class Color
{
public:
	
	Color() {};
	Color(int r, int g, int b) { mColor = al_map_rgb(r, g, b); }
	~Color() {};

private:
	
	ALLEGRO_COLOR mColor;
	ALLEGRO_COLOR getColor() const { return mColor; }
};