
#include "RoboCatPCH.h"
#include "allegro_wrapper_functions-main/GraphicsLibrary.h"
#include "Bullet.h"
#include <chrono>

#if _WIN32

const string SPRITES_PATH = "images/";
const string BACKGROUND_FILENAME = "BackgroundWhite.jpg";
const string BULLET_FILENAME = "PlayerSquare.png";

const float RESOLUTION_X = 1920.0;
const float RESOLUTION_Y = 1080.0;

int main(int argc, const char** argv)
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#else
const char** __argv;
int __argc;
int main(int argc, const char** argv)
{
	__argc = argc;
	__argv = argv;
#endif

	clock_t start, end;
	float last_time;
	float dt = 0;
	start = clock();

	float playerSpeed = 0.1;
	float playerPositionX = RESOLUTION_X / 2;
	float playerPositionY = RESOLUTION_Y / 10;

	GraphicsLibrary GL(RESOLUTION_X, RESOLUTION_Y);
	GL.init("background");

	GL.loadImage(SPRITES_PATH + BACKGROUND_FILENAME, "background");
	GL.drawImage("background", 0.0, 0.0);
	
	GL.loadImage(SPRITES_PATH + SPRITES_PATH, "player1");
	GL.drawImage("player1", playerPositionX, playerPositionY);

	GL.render();

	SocketUtil::StaticInit();

//	Bullet bullet = new Bullet();

	while (true)
	{
		end = clock();
		last_time = ((float)end - start) / CLOCKS_PER_SEC;
		dt = (float)end / CLOCKS_PER_SEC - last_time;
		std::cout << dt << std::endl;

		//playerPositionX += playerSpeed * dt;
		//GL.drawImage("player1", playerPositionX, playerPositionY);

		//GL.render();
	}

	SocketUtil::CleanUp();

	return 0;
}
