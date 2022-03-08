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

//SocketUtil::CleanUp();