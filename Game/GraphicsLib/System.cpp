#include "System.h"

System::System()
{
	mpGraphicsSystem = nullptr;
	mpISystem = nullptr;
}

System::~System()
{
}

void System::init(int width, int height)
{
	//create the GSystem
	mpGraphicsSystem = new GraphicsSystem();
	mpGraphicsSystem->init(width,height);

	mpISystem = new InputSystem();
	mpISystem->init();

	//install allegro dependencies
	if (!al_install_keyboard())
		std::cout << "keyboard install failed";
	if (!al_install_mouse())
		std::cout << "mouse install failed";

}

void System::cleanup()
{
	//destroy GSystem
	mpGraphicsSystem->cleanup();
	delete(mpGraphicsSystem);

	mpISystem->cleanup();
	delete(mpISystem);
}

void System::update()
{
	mpISystem->update();
}

/*
int System::getMouseState()
{
	al_get_mouse_state(&mMouseState);

	//if right clicked, return true
	if (mMouseState.buttons & 1)
	{
		return 1;
	}
	else if(mMouseState.buttons & 2)
	{
		return 2;
	}
	else
	{
		return 0;
	}
}

string System::getKeyState()
{
	al_get_keyboard_state(&mKeyState);

	//return the current key being pressed
	if (al_key_down(&mKeyState, ALLEGRO_KEY_F))
	{
		return "f";
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_S))
	{
		return "s";

	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_ENTER))
	{
		return "enter";
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_ESCAPE))
	{
		return "esc";
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_SPACE))
	{
		return "space";
	}
	else
	{
		return " ";
	}
}

float System::getMouseX()
{
	return mMouseState.x;
}

float System::getMouseY()
{
	return mMouseState.y;
}*/
