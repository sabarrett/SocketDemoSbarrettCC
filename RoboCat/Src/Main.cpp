#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "RoboCatPCH.h"
#include "..\Game.h"



int main(int argc, const char** argv)
{
	Game game;
	game.Update();

	return 0;
}


////ALLEGRO_EVENT_QUEUE mpEventQueue = al_create_event_queue();
//
//bool running = true;
//
////Init allegro
//if (!al_init())
//{
//	std::cout << "error initting Allegro\n";
//	system("pause");
//	return false;
//}
//
////Init image addon
//if (!al_init_image_addon())
//{
//	std::cout << "error initting image add-on\n";
//	system("pause");
//	return false;
//}
//
////Init font add on
//if (!al_init_font_addon())
//{
//	std::cout << "error initting font add-on\n";
//	system("pause");
//	return false;
//}
//
////Init ttf add on
//if (!al_init_ttf_addon())
//{
//	std::cout << "error initting ttf add-on\n";
//	system("pause");
//	return false;
//}
//if (!al_init_primitives_addon())
//{
//	std::cout << "error initting primitive add-on\n";
//	system("pause");
//	return false;
//}
//
////Init keyboard
//if (!al_install_keyboard())
//{
//	std::cout << "error installing Allegro keyboard plugin\n";
//	system("pause");
//	return false;
//}
//
//
//
//ALLEGRO_DISPLAY* display = al_create_display(800, 640);
//SocketUtil::StaticInit();
//
//ALLEGRO_COLOR paddleColor = al_map_rgb(255, 255, 255);
//
//
//ALLEGRO_COLOR backgroundColor = al_map_rgb(0, 0, 0);
//
//Paddle paddle1 = Paddle(20, 100, paddleColor);
//paddle1.SetPosition(30, 100);
//
//Paddle paddle2 = Paddle(20, 100, paddleColor);
//paddle2.SetPosition(al_get_display_width(display) - 50, 100);
//
//ALLEGRO_KEYBOARD_STATE* state = new ALLEGRO_KEYBOARD_STATE();
//
//al_get_keyboard_state(state);
//
//while (running)
//{
//	al_get_keyboard_state(state);
//
//	if (al_key_down(state, ALLEGRO_KEY_SPACE))
//	{
//		running = false;
//	}
//
//
//	int moveState = 0;
//
//	if (al_key_down(state, ALLEGRO_KEY_UP))
//	{
//		moveState += 1;
//
//		if (moveState > 1)
//			moveState = 1;
//	}
//
//	if (al_key_down(state, ALLEGRO_KEY_DOWN))
//	{
//		moveState -= 1;
//
//		if (moveState < -1)
//			moveState = -1;
//	}
//
//	int height = paddle1.GetPosY();
//
//	if (moveState == 1)
//	{
//		if (height > 0)
//		{
//			paddle1.SetPosition(paddle1.GetPosX(), height - 2);
//		}
//	}
//
//	if (moveState == -1)
//	{
//		if (height + paddle1.GetDimsW() < al_get_display_height(display))
//		{
//			paddle1.SetPosition(paddle1.GetPosX(), height + 2);
//		}
//	}
//
//
//
//
//
//
//	al_clear_to_color(backgroundColor);
//	paddle1.Render();
//	paddle2.Render();
//	al_flip_display();
//
//}
//
//
//SocketUtil::CleanUp();