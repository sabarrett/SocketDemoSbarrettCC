#pragma once
#include <allegro5/color.h>

struct position {
	float x, y;
};

class ball 
{
private:
	float mDirX, mDirY;
	int mRadius;
	ALLEGRO_COLOR mColor;

public:
	ball();
	ball(float movX, float movY, int r, ALLEGRO_COLOR c);
	~ball();

	position* pos;
	int id;

	void Render();

	void SetMovementVector(float x, float y);
	void SetRadius(int r);
	void SetColor(ALLEGRO_COLOR c);

	float GetPosX() { return pos->x; };
	float GetPosY() { return pos->y; };

	float GetDirX() { return mDirX; };
	float GetDirY() { return mDirY; };

	int GetRadius() { return mRadius; };

};