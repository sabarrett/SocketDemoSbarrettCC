#include "GraphicsSystem.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include <iostream>
#include <cassert>
#include <string>
#include <stdlib.h>

#include <PerformanceTracker.h>
#include <MemoryTracker.h>
#include <Timer.h>

GraphicsSystem::GraphicsSystem()
{
	mpDisplay = nullptr;
}

GraphicsSystem::~GraphicsSystem()
{
	cleanup();
}

void GraphicsSystem::init(int width, int height)
{
	//Initialize all necassary addons

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

	//create the display
	mpDisplay = al_create_display(width, height);
	assert(mpDisplay);
}

void GraphicsSystem::cleanup()
{
	al_destroy_display(mpDisplay);
	mpDisplay = nullptr;
}

int GraphicsSystem::getHeight()
{
	return al_get_display_height(mpDisplay);
}

int GraphicsSystem::getWidth()
{
	return al_get_display_width(mpDisplay);
}

void GraphicsSystem::flip()
{
	al_flip_display();
}

void GraphicsSystem::draw(int x, int y, Sprite sprite, float scale)
{
	al_set_target_bitmap(al_get_backbuffer(mpDisplay)); //Set the target to the display
	//Draw the scaled sprite
	al_draw_scaled_bitmap(sprite.getBuffer().mpBitmap, sprite.getSouceLoc()[0], sprite.getSouceLoc()[1], sprite.getWidth(), sprite.getHeight(),x, y, sprite.getWidth() * scale, sprite.getHeight() * scale,0);
}

void GraphicsSystem::draw(GraphicsBuffer* backBuffer, int x, int y, Sprite sprite, float scale)
{
	al_set_target_bitmap(backBuffer->mpBitmap); //Set the target to the inputed bitmap
	//Draw the scaled sprite
	al_draw_scaled_bitmap(sprite.getBuffer().mpBitmap, sprite.getSouceLoc()[0], sprite.getSouceLoc()[1], sprite.getWidth(), sprite.getHeight(), x, y, sprite.getWidth() * scale, sprite.getHeight() * scale, 0);
}

void GraphicsSystem::draw(GraphicsBuffer* backBuffer, int x, int y, float scale)
{
	al_set_target_bitmap(al_get_backbuffer(mpDisplay)); //Set the target to the display
	//draw that bitmap to screen at scale
	al_draw_scaled_bitmap(backBuffer->mpBitmap, 0, 0, backBuffer->getWidth(), backBuffer->getHeight(), x, y, backBuffer->getWidth() * scale, backBuffer->getHeight() * scale, 0);
}

void GraphicsSystem::saveBuffer(string filename)
{
	al_save_bitmap(filename.c_str(), al_get_backbuffer(mpDisplay));
}

void GraphicsSystem::saveBuffer(GraphicsBuffer* backBufferToSave, string filename)
{
	al_save_bitmap(filename.c_str(), backBufferToSave->mpBitmap);
}
