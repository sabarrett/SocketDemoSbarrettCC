#pragma once
#include <allegro5/color.h>
#include <allegro5/allegro_font.h>
#include <string>

class Score
{
private:
	int mOriginX, mOriginY;
	ALLEGRO_FONT* mFont;
	ALLEGRO_COLOR mColor;
public:
	Score(int x, int y, ALLEGRO_COLOR c);
	~Score();

	std::string str;

	void Render();

	void SetPosition(int x, int y);
};