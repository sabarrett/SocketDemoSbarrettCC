#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <vector>
#include <string>
#include <map>
#include <utility>

using std::vector;
using std::pair;
using std::string;
using std::map;

class GraphicsSystems
{
public:

	GraphicsSystems(); //constructor
	~GraphicsSystems(); //destructor


	bool Init(int mWidth, int mHeight);
	void Draw(int locX, int locY, ALLEGRO_BITMAP* bitmap); //Draw to current backbuffer

	
    void Flip() { al_flip_display(); };//flips display

	int getmScreenHeight() { return mScreenHeight; };
	int getmScreenWidth() { return mScreenWidth; };
	ALLEGRO_DISPLAY* getmpDisplay() { return mpDisplay; };

private:

	int mScreenWidth;

	int mScreenHeight;

	ALLEGRO_DISPLAY* mpDisplay;
};