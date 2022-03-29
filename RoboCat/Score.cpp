#include "Score.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>

Score::Score(int x, int y, ALLEGRO_COLOR c) :
	mOriginX(x),
	mOriginY(y),
	mColor(c)
{
	mFont = al_load_font("OpenSans-SemiboldItalic.ttf", 28, 0);
}

Score::~Score()
{
}

void Score::Render()
{
	al_draw_text(mFont, mColor, mOriginX, mOriginY, 0, str.c_str());
}

void Score::SetPosition(int x, int y)
{
	mOriginX = x;
	mOriginY = y;
}
