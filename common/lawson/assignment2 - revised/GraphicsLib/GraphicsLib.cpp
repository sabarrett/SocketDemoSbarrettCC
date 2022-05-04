#include "GraphicsLib.h"

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
#include <vector2D.h>

using namespace std;

ALLEGRO_DISPLAY* initAllegro()
{
	if (!al_init())
	{
		cout << "error initting Allegro\n";
		return nullptr;
	}
	if (!al_init_image_addon())
	{
		cout << "error - Image Add-on not initted\n";
		return nullptr;
	}
	if (!al_init_font_addon())
	{
		cout << "error - Font Add-on not initted\n";
		return nullptr;
	}
	if (!al_init_ttf_addon())
	{
		cout << "error - TTF Add-on not initted\n";
		return nullptr;
	}
	if (!al_init_primitives_addon())
	{
		cout << "error - primitives Add-on not initted\n";
		return nullptr;
	}
	if (!al_install_audio())
	{
		cout << "error - Audio Add-on not initted\n";
		//return 1;
	}
	if (!al_init_acodec_addon())
	{
		cout << "error - Audio Codec Add-on not initted\n";
		//return 1;
	}
	if (!al_reserve_samples(1))
	{
		cout << "error - samples not reserved\n";
		//return 1;
	}

	const int DISP_WIDTH = 800;
	const int DISP_HEIGHT = 600;


	ALLEGRO_DISPLAY* display = al_create_display(DISP_WIDTH, DISP_HEIGHT);
	assert(display);

	al_install_keyboard();
	al_install_mouse();
	return display;
}

void drawScene(ALLEGRO_DISPLAY* pDisplay)
{
	const string ASSET_PATH = "..\\..\\..\\common\\assets\\";
	const string BACKGROUND_FILENAME = "axamer-lizum.png";
	const string QUIMBY_FILENAME = "mayor_quimby.png";
	const string FONT_FILENAME = "cour.ttf";
	const int FONT_SIZE = 24;
	const string SAMPLE_FILENAME = "clapping.wav";

	ALLEGRO_BITMAP* bitmap = al_load_bitmap((ASSET_PATH + BACKGROUND_FILENAME).c_str());
	assert(bitmap);
	ALLEGRO_BITMAP* quimby = al_load_bitmap((ASSET_PATH + QUIMBY_FILENAME).c_str());
	assert(quimby);

	ALLEGRO_FONT* cour_font = al_load_ttf_font((ASSET_PATH + FONT_FILENAME).c_str(), FONT_SIZE, 0);
	assert(cour_font);

	const ALLEGRO_COLOR WHITE = al_map_rgb(255, 255, 255);
	const ALLEGRO_COLOR BLACK = al_map_rgb(0, 0, 0);
	const ALLEGRO_COLOR BLACK_TRANSPARENT = al_map_rgba(0, 0, 0, 200);
	const ALLEGRO_COLOR PURPLE = al_map_rgb(128, 64, 212);

	ALLEGRO_SAMPLE* sample = al_load_sample((ASSET_PATH + SAMPLE_FILENAME).c_str());
	assert(sample);

	al_play_sample(sample, 1.0f, ALLEGRO_AUDIO_PAN_NONE, 1.0f, ALLEGRO_PLAYMODE_LOOP, nullptr);


	al_clear_to_color(WHITE);

	al_draw_bitmap(bitmap, 0, 0, 0);
	const int CIRCLE_RADIUS = 150;
	const int LOC1_X = 400;
	const int LOC1_Y = 300;

	al_draw_filled_circle(LOC1_X, LOC1_Y, CIRCLE_RADIUS, BLACK);
	al_draw_text(cour_font, WHITE, LOC1_X, LOC1_Y, ALLEGRO_ALIGN_CENTER, "Welcome to Allegro!");

	const int LOC2_X = 200;
	const int LOC2_Y = 500;
	al_draw_filled_circle(LOC2_X, LOC2_Y, CIRCLE_RADIUS, BLACK_TRANSPARENT);
	al_draw_text(cour_font, PURPLE, LOC2_X, LOC2_Y, ALLEGRO_ALIGN_CENTER, "Welcome to Allegro!");

	const int LOC3_X = 500;
	const int LOC3_Y = 400;
	int sourceWidth = al_get_bitmap_width(quimby);
	int sourceHeight = al_get_bitmap_height(quimby);
	const float SCALE_FACTOR = 0.75f;
	al_draw_scaled_bitmap(quimby, 0, 0, sourceWidth, sourceHeight, LOC3_X, LOC3_Y, sourceWidth * SCALE_FACTOR, sourceHeight * SCALE_FACTOR, 0);

	al_flip_display();

	al_destroy_bitmap(bitmap);
	al_destroy_bitmap(quimby);
	//al_destroy_sample(sample);
	al_destroy_font(cour_font);
}

void cleanupAllegro(ALLEGRO_DISPLAY* pDisplay)
{
	al_destroy_display(pDisplay);

}

ALLEGRO_EVENT_QUEUE* initEventQueue()
{
	ALLEGRO_EVENT_QUEUE* pQueue = al_create_event_queue();
	if (pQueue == NULL)
	{
		cout << "InputSystem: unable to create Event Queue!\n";
		assert(false);
	}
	else
	{
		al_register_event_source(pQueue, al_get_keyboard_event_source());
		al_register_event_source(pQueue, al_get_mouse_event_source());
	}
	return pQueue;
}

bool processEventQueue(ALLEGRO_EVENT_QUEUE* theQueue)
{
	ALLEGRO_EVENT theEvent;

	while (al_get_next_event(theQueue, &theEvent))
	{
		switch (theEvent.type)
		{
		case ALLEGRO_EVENT_KEY_DOWN:
		case ALLEGRO_EVENT_KEY_UP:
			if (theEvent.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				cout << theEvent.keyboard.keycode << " Down\n";
			}
			else if (theEvent.type == ALLEGRO_EVENT_KEY_UP)
			{
				cout << theEvent.keyboard.keycode << " Up\n";
				if (theEvent.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					return true;
			}
			break;
		case ALLEGRO_EVENT_MOUSE_AXES:
			{
				Vector2D vec(theEvent.mouse.x, theEvent.mouse.y);
				cout << vec << endl;
			}
			break;
		case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
		case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
			if (theEvent.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			{
				cout << theEvent.mouse.button << " Mouse Down\n";
			}
			else if (theEvent.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
			{
				cout << theEvent.mouse.button << " Mouse Up\n";
			}
			Vector2D vec(theEvent.mouse.x, theEvent.mouse.y);
			cout << vec << endl;
			break;
		}
	}
	return false;
}

void cleanupEventQueue(ALLEGRO_EVENT_QUEUE* theQueue)
{
	al_unregister_event_source(theQueue, al_get_keyboard_event_source());
	al_unregister_event_source(theQueue, al_get_mouse_event_source());
	al_destroy_event_queue(theQueue);
}
