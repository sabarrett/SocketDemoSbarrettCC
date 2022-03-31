#pragma once
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <vector>
#include <string>

class GraphicsSystems
{
public:

	GraphicsSystems(); //constructor
	~GraphicsSystems(); //destructor


	bool init(int mWidth, int mHeight);

	void cleanup();
	void draw(int locX, int locY, ALLEGRO_BITMAP* bitmap); //draw to current backbuffer

	
    void flip();//flips display

	int getmScreenHeight() { return mScreenHeight; };
	int getmScreenWidth() { return mScreenWidth; };
	ALLEGRO_DISPLAY* getmpDisplay() { return mpDisplay; };
	
	//mutators
	//void setmInited(bool torf) { mInited = torf; };

private:

	int mScreenWidth;

	int mScreenHeight;

	

	ALLEGRO_DISPLAY* mpDisplay;

	
};