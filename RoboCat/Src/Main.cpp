
#include "RoboCatPCH.h"
#include "allegro.h"
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

	

	ALLEGRO_DISPLAY* display;
	if (!al_init())
	{
		printf("fuck");
	}
	display = al_create_display(800, 600);

	system("pause");

	//socket stuff here
	SocketUtil::StaticInit(); // socket initialization

	//create a server
	UDPSocketPtr serverSocket;

	SocketUtil::CleanUp(); // socket cleanup

	return 0;
}
