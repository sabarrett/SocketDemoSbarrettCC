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
	mFont = al_create_builtin_font();
	
	assert(mFont);
}

Score::~Score()
{
}

void Score::Render()
{
	std::string str = std::to_string(points);

	al_draw_text(mFont, mColor, mOriginX, mOriginY, 0, str.c_str());
}

void Score::SetPosition(int x, int y)
{
	mOriginX = x;
	mOriginY = y;
}
