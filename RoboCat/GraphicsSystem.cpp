#include "GraphicsSystem.h"

/*
Author: Ryan Littleton, with help from the rest of the class
Class : GPR-250-71
Assignment : Assignment 2
*/

GraphicsSystem::GraphicsSystem(int width, int height) 
{
	mDisplayWidth = width;
	mDisplayHeight = height;
}
GraphicsSystem::~GraphicsSystem() 
{
	cleanUp();
	mDisplayWidth = DISP_WIDTH;
	mDisplayHeight = DISP_HEIGHT;
	mpDisplay = nullptr;
}

void GraphicsSystem::init()
{
	if (!al_init())
	{
		cout << "error initting Allegro\n";
	}
	if (!al_init_image_addon())
	{
		cout << "error - Image Add-on not initted\n";
	}
	if (!al_init_font_addon())
	{
		cout << "error - Font Add-on not initted\n";
	}
	if (!al_init_ttf_addon())
	{
		cout << "error - TTF Add-on not initted\n";
	}
	if (!al_init_primitives_addon())
	{
		cout << "error - primitives Add-on not initted\n";
	}
	if (!al_install_audio())
	{
		cout << "error - Audio Add-on not initted\n";
	}
	if (!al_init_acodec_addon())
	{
		cout << "error - Audio Codec Add-on not initted\n";
	}
	if (!al_reserve_samples(1))
	{
		cout << "error - samples not reserved\n";
	}

	mpDisplay = al_create_display( mDisplayWidth, mDisplayHeight );
	assert( mpDisplay );
}

void GraphicsSystem::cleanUp()
{
	al_destroy_display(mpDisplay);
}

int GraphicsSystem::getHeight()
{
	return al_get_display_height(mpDisplay);
}

int GraphicsSystem::getWidth()
{
	return al_get_display_width(mpDisplay);
}

ALLEGRO_BITMAP* GraphicsSystem::getBackBuffer() 
{ 
	return al_get_backbuffer(mpDisplay); 
}

void GraphicsSystem::flip() 
{
	al_flip_display();
}

void GraphicsSystem::draw(int xLoc, int yLoc, Sprite* sprite, float scale)
{
	al_set_target_bitmap(al_get_backbuffer(mpDisplay));
	ALLEGRO_BITMAP* bitmap = sprite->getGraphicsBuffer()->getGraphicsBuffer();
	float width = sprite->getWidth();
	float height = sprite->getHeight();
	float x = sprite->getLocX();
	float y = sprite->getLocY();
	al_draw_scaled_bitmap(bitmap, x, y, width, height, xLoc, yLoc, width * scale, height * scale, 0);
}

void GraphicsSystem::draw(GraphicsBuffer targetBuffer, int xLoc, int yLoc, Sprite sprite, float scale)
{
	al_set_target_bitmap(targetBuffer.getGraphicsBuffer());
	ALLEGRO_BITMAP* bitmap = sprite.getGraphicsBuffer()->getGraphicsBuffer();
	float width = sprite.getHeight();
	float height = sprite.getHeight();
	float x = sprite.getLocX();
	float y = sprite.getLocY();
	cout << scale << endl;
	al_draw_scaled_bitmap(bitmap, x, y, width, height, xLoc, yLoc, width * scale, height * scale, 0);
	al_set_target_bitmap(al_get_backbuffer(mpDisplay));
}

void GraphicsSystem::writeText(int xLoc, int yLoc, Font font, Color color, std::string text)
{
	al_draw_text(font.getFont(), color.getColor(), xLoc, yLoc, ALLEGRO_ALIGN_LEFT, text.c_str());
}

void GraphicsSystem::writeText(GraphicsBuffer targetBuffer, int xLoc, int yLoc, Font font, Color color, std::string text)
{
	al_set_target_bitmap(targetBuffer.getGraphicsBuffer());
	al_draw_text(font.getFont(), color.getColor(), xLoc, yLoc, ALLEGRO_ALIGN_LEFT, text.c_str());
	al_set_target_bitmap(al_get_backbuffer(mpDisplay));
}

void GraphicsSystem::saveBuffer(GraphicsBuffer targetBuffer, std::string filename)
{
	al_save_bitmap(filename.c_str(), targetBuffer.getGraphicsBuffer());
}

void GraphicsSystem::saveBuffer(ALLEGRO_BITMAP* targetBuffer, std::string filename)
{
	al_save_bitmap(filename.c_str(), targetBuffer);
}
