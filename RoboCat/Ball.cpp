#include "Ball.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>

ball::ball()
{

}

ball::ball(float movX, float movY, int r, ALLEGRO_COLOR c) :
	mRadius(r),
	mColor(c),
	mDirX(movX),
	mDirY(movY)
{

}

ball::~ball()
{

}

void ball::Render()
{
	al_draw_filled_circle(pos->x, pos->y, mRadius, mColor);
}


void ball::SetMovementVector(float x, float y)
{
	mDirX = x;
	mDirY = y;
}

void ball::SetRadius(int r)
{
	mRadius = r;
}

void ball::SetColor(ALLEGRO_COLOR c)
{
	mColor = c;
}