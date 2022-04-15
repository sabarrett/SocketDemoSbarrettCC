#include "System.h"

System::System()
{
	mGraphicsSystem = new GraphicsSystem(DISP_WIDTH, DISP_HEIGHT);
	mInput = new InputSystem();
}

System::~System()
{
	cleanup();
}

void System::init()
{
	mGraphicsSystem->init(); // this also contains init allegro and should always be called before anything else	
	
	if (!al_install_mouse())
	{
		cout << "error - Mouse Add-on not installed\n";
	}
	if (!al_install_keyboard())
	{
		cout << "error - Keyboard Add-on not installed\n";
	}
	mInput->init();
	mIsDeleted = false;
}

void System::cleanup()
{
	if (mIsDeleted == false)
	{
		delete mGraphicsSystem;
		mGraphicsSystem = nullptr;

		mIsDeleted = true;
	}
}