
#include "RoboCatPCH.h"
#include "allegro_wrapper_functions-main/GraphicsLibrary.h"

#if _WIN32


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

	const float SCREEN_X = 1000;
	const float SCREEN_Y = 500;
	const string FILE_PATH = "../images/";
	const string BACKGROUND = "background.bmp";

	GraphicsLibrary gl(SCREEN_X, SCREEN_Y);
	gl.init();

	gl.loadImage(FILE_PATH + BACKGROUND, BACKGROUND);
	gl.drawImage(BACKGROUND, 0, 0);
	gl.render();

	SocketUtil::StaticInit();
	// main game loop
	while (true)
	{
		;
	}

	SocketUtil::CleanUp();

	return 0;
}
