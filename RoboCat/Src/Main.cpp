
#include "RoboCatPCH.h"
#include "InputSystem.h"
#include "GraphicsLibrary.h"
#include "Colour.h"

//Some functions to make shapes for the game objects to track and send info about
//al_draw_circle
//al_draw_triangle
//al_draw_rectangle

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

	SocketUtil::StaticInit();

	//while (activeConnection == false)
	InputSystem mInputSystem; 
	bool activeConnection; // = true

	//something to start up the connection (info from assignment 1?)
	//then add the keys for making the objects spawn (3 objects, 3 keys)
		//need something to track the objects made? idk if there is something i can use?
	//each update send out the information about the object type and its position
		//then prepare to receive it as well?
	
	
	SocketUtil::CleanUp();

	return 0;
}