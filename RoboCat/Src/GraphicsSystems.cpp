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
	cleanup();
};

bool GraphicsSystems::init(int width, int height)
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

void GraphicsSystems::cleanup()
{
	al_destroy_display(mpDisplay);
	mpDisplay = nullptr;
}


void GraphicsSystems::draw(int locX, int locY, ALLEGRO_BITMAP* bitmap)//draws to the back buffer
{
	al_draw_bitmap(bitmap,locX, locY, 0);
}

void GraphicsSystems::flip()
{
   al_flip_display();
}



