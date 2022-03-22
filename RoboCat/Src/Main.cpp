///
/// This code is based on the code provided by the instructor Scott Barrett, as well
/// as wrapper functions made by Adel Talhouk (the latter having header comments indicating such)
/// unless otherwise stated, the rest of the work was done by Brandon Boras (bb) and Nicholas Perell
/// 

// standard libraries
#include <thread>

// libraries provided to us
#include "RoboCatPCH.h"
#include "allegro_wrapper_functions-main/GraphicsLibrary.h"

// custom libraries


#if _WIN32


///
///          TODO
///  X - Synchronize game objects over a socket.
///  X - Create a two-player networked game with shared world state.
///  X - Use Memory Streams and different Packet Header types to synchronize the state between the two instances.
///  X - At least 10 synchronized objects in the world at once.
///  X - At least 3 different kinds of game objects.
///  X - Involve creating and destroying objects, which is then synchronized between clients.
///  X - Use TCP and UDP appropriately, (for deltas and world state respectively) 


const float SCREEN_X = 1000;
const float SCREEN_Y = 500;
const string FILE_PATH = "../images/";
const string BACKGROUND = "background.jpg";

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
	
	string userAnswer;
	bool userIsHosting;

	std::cout << "Hello, would you like to create (c) or join (j) a match of this game?\n";
	do
	{
		std::cin >> userAnswer;
	} while (!(userAnswer == "c" || userAnswer == "C" || userAnswer == "j" || userAnswer == "J"));

	userIsHosting = (userAnswer == "c" || userAnswer == "C");

	if (userIsHosting)
	{
		std::cout << "Thank you for your answer, we will now wait for someone to join the game...";


	}
	else // try to connect to a host
	{

	}



	//  Graphics init
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
