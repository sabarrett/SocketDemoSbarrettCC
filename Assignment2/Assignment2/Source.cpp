#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>
#include "Player.h"
#include "Crosshair.h"
#include "Globals.h"

bool init();
bool loadMedia();
void close();

const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 540;

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer;

Player player1;
Crosshair crosshair1;

bool init()
{
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Failure initializing SDL.  Error: %s\n", SDL_GetError());
	}
	else
	{
		gWindow = SDL_CreateWindow( "Assignment 2/3 Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Failure initializing Window.  Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			// create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Failure creaing renderer.  Error: %s\n", SDL_GetError());
			}
			else
			{
				SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0xFF);

				// IMG_Init returns a number, IMG_INIT_PNG == 2.  bitwise & to compare
				if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
				{
					printf("Failure initializing SDL_image.  Error %s\n", SDL_GetError());
					success = false;
				}

				SDL_ShowCursor(false);
			}
		}
	}
	return success;
}

bool loadMedia()
{
	bool success = true;

	if (!Player::playerTexture.loadFromFile("images/player.png"))
	{
		printf("Failed to load player texture");
		success = false;
	}

	if (!Crosshair::crosshairTexture.loadFromFile("images/crosshair.png"))
	{
		printf("Failed to load crosshair texture");
		success = false;
	}

	return success;
}

void close()
{
	Player::playerTexture.free();
	Crosshair::crosshairTexture.free();

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;

	IMG_Quit();
	SDL_Quit();
}

// SDL requires this type of main function
int main(int argc, char* args[])
{
	if (!init())
	{
		printf("init failure");
	}
	else
	{
		if (!loadMedia())
		{
			printf("loadMedia failure");
		}
		else
		{
			bool quit = false;

			SDL_Event e;

			Color clientColor = Color(rand() % 255 + 100, rand() % 255 + 100, rand() % 255 + 100);
			player1.setColor(clientColor);
			crosshair1.setColor(clientColor);

			while (!quit)
			{
				while (SDL_PollEvent(&e) != 0)
				{
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					player1.handleEvent(e);
					crosshair1.handleEvent(e);
				}

				// fill screen with SDL_SetRenderDrawColor
				SDL_RenderClear(gRenderer);

				// update textures
				player1.update();
				crosshair1.update();

				// render textures
				player1.render();
				crosshair1.render();

				// update screen
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	close();

	return 0;
}