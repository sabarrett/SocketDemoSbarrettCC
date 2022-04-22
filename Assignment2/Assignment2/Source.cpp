#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include "LTexture.h"

bool init();
// SDL_Surface* loadSurface();
// SDL_Texture* loadTexture(std::string path);
bool loadMedia();
void close();

const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 540;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

LTexture playerTexture = LTexture(&gRenderer);

LTexture crosshair = LTexture(&gRenderer);

LTexture bullets[50];
int bulletArrSize = 0;

// SDL_Surface* gScreenSurface = NULL;
// SDL_Surface* gHelloWorld = NULL;

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

	if (!playerTexture.loadFromFile("images/player.png"))
	{
		printf("Failed to load player texture");
		success = false;
	}

	if (!crosshair.loadFromFile("images/crosshair.png"))
	{
		printf("Failed to load crosshair texture");
		success = false;
	}

	return success;
}

void close()
{
	playerTexture.free();
	crosshair.free();
	for(int i = 0; i < bulletArrSize; i++)
	{
		bullets[i].free();
	}

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

			playerTexture.setPos(Vec2D(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));

			SDL_Event e;

			while (!quit)
			{
				int mouseX, mouseY;

				while (SDL_PollEvent(&e) != 0)
				{
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					else if (e.type == SDL_MOUSEMOTION)
					{
						SDL_GetMouseState(&mouseX, &mouseY);

						playerTexture.faceTowards(Vec2D(mouseX, mouseY));
						crosshair.setPos(Vec2D(mouseX - crosshair.getSize().x / 2, mouseY - crosshair.getSize().y / 2));
					}
					else if (e.type == SDL_MOUSEBUTTONDOWN)
					{
						double degrees = (playerTexture.getAngle() + 180.0) * M_PI / 180;
						playerTexture.setVelocity(
							Vec2D(
								cos(degrees) / 3.0, sin(degrees) / 3.0
							)
						);

						if (bulletArrSize < 50)
						{
							LTexture bullet = LTexture(&gRenderer);
							bullet.loadFromFile("images/bullet.png");
							bullet.setPos(Vec2D(playerTexture.getPosition().x, playerTexture.getPosition().y));
							bullet.setRenderer(&gRenderer);
							bullets[bulletArrSize] = bullet;
							bulletArrSize++;
						}
					}
				}

				// fill screen with SDL_SetRenderDrawColor
				SDL_RenderClear(gRenderer);

				// update textures
				playerTexture.update();

				// render textures
				playerTexture.render();
				crosshair.render();
				for (int i = 0; i < bulletArrSize; i++)
				{
					bullets[i].render();
				}

				// update screen
				SDL_RenderPresent(gRenderer);

				/*
				SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);

				SDL_UpdateWindowSurface(gWindow);
				*/
			}
		}
	}

	close();

	return 0;
}