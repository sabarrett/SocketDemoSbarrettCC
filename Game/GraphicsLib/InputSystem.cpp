#include "InputSystem.h"

InputSystem::InputSystem()
{
	mpEventSystem = EventSystem::getInstance();
}

InputSystem::~InputSystem()
{

}

void InputSystem::init()
{
	//install allegro dependencies
	if (!al_install_keyboard())
		std::cout << "keyboard install failed";
	if (!al_install_mouse())
		std::cout << "mouse install failed";
}

void InputSystem::cleanup()
{
	//uninstall allegro dependencies
	al_uninstall_keyboard();
	al_uninstall_mouse();
}

void InputSystem::update()
{
	al_get_mouse_state(&mMouseState); //get allegro mouse state

	//if right clicked, return true
	if (mMouseState.buttons & 1)
	{
		//fire mouse event with right click and mouse position
		mpEventSystem->fireEvent(MouseEvent(1, mMouseState.x, mMouseState.y));
	}
	else if (mMouseState.buttons & 2)
	{
		//fire mouse event with left click and mouse position
		mpEventSystem->fireEvent(MouseEvent(2, mMouseState.x, mMouseState.y));
	}

	al_get_keyboard_state(&mKeyState);//get allegro key state

	if (al_key_down(&mKeyState, ALLEGRO_KEY_ENTER))
	{
		//fire key event with enter click
		mpEventSystem->fireEvent(KeyEvent("enter"));
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_ESCAPE))
	{
		//fire key event with esc click
		mpEventSystem->fireEvent(KeyEvent("esc"));
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_SPACE))
	{
		//fire key event with space click
		mpEventSystem->fireEvent(KeyEvent("space"));
	}
}
