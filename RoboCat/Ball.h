#pragma once
#include <allegro5/color.h>

struct position {
	float x, y;

};

class ball 
{
private:
	float mOriginX, mOriginY;
	float mDirX, mDirY;
	int mRadius;
	ALLEGRO_COLOR mColor;

public:

	ball(float movX, float movY, int r, ALLEGRO_COLOR c);
	~ball();

	void Render();

	void SetPosition(float x, float y);
	void SetMovementVector(float x, float y);
	void SetRadius(int r);
	void SetColor(ALLEGRO_COLOR c);

	float GetPosX() { return mOriginX; };
	float GetPosY() { return mOriginY; };

	float GetDirX() { return mDirX; };
	float GetDirY() { return mDirY; };

	int GetRadius() { return mRadius; };

};