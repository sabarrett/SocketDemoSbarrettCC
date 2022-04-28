#include "GraphicsSystems.h"
//#include "RoboCatPCH.h"

GraphicsSystems::GraphicsSystems()
{

	mScreenWidth = 600;

	mScreenHeight = 800;

	mpDisplay = nullptr;
};

GraphicsSystems::~GraphicsSystems()
{
	al_destroy_display(mpDisplay);
	mpDisplay = nullptr;
};

bool GraphicsSystems::Init(int width, int height)
{
	if (!al_init())
	{
		//std::cout << "error initting Allegro\n";
		return false;
	}
	else if (!al_init_image_addon())
	{
		return false;
	}

	mScreenWidth = width;
	mScreenHeight = height;

	mpDisplay = al_create_display(mScreenWidth, mScreenHeight);

	return true;
}

void GraphicsSystems::Draw(int locX, int locY, ALLEGRO_BITMAP* bitmap)//draws to the back buffer
{
	al_draw_bitmap(bitmap,locX, locY, 0);
}



