#pragma once
#include <allegro5/color.h>

class Paddle
{

	int mOriginX, mOriginY;
	int mLength, mWidth;
	ALLEGRO_COLOR mColor;

public:

	Paddle(int l, int w, ALLEGRO_COLOR c);
	~Paddle();

	void Render();

	void SetPosition(int x, int y);
	void SetPosition(int y) { mOriginY = y; };
	void SetDims(int l, int w);
	void SetColor(ALLEGRO_COLOR c);

	int GetPosX() { return mOriginX; };
	int GetPosY() { return mOriginY; };

	int GetDimsL() { return mLength; };
	int GetDimsW() { return mWidth; };

};

