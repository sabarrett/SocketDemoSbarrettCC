#include "Paddle.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>

Paddle::Paddle(int l, int w, ALLEGRO_COLOR c) :
	mLength(l),
	mWidth(w),
	mColor(c)

{

}

Paddle::~Paddle()
{

}

void Paddle::Render()
{
	al_draw_filled_rectangle(mOriginX, mOriginY, mOriginX + mLength, mOriginY + mWidth, mColor);
	//al_draw_filled_rectangle(100, 100, 100 + 15, 100 + 15, al_map_rgb(155, 255, 155));
}

void Paddle::SetPosition(int x, int y)
{
	mOriginX = x;
	mOriginY = y;
}

void Paddle::SetDims(int l, int w)
{
	mLength = l;
	mWidth = w;
}

void Paddle::SetColor(ALLEGRO_COLOR c)
{
	mColor = c;
}