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
	std::cout << "init InputSystem" << endl;

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
	//std::cout << "update InputSystem" << endl;
	al_get_mouse_state(&mMouseState); //get allegro mouse state

	//if right clicked, return true
	if (mMouseState.buttons & 1)
	{
		//fire mouse event with right click and mouse position
		mpEventSystem->fireEvent(MouseEvent(1, mMouseState.x, mMouseState.y));
		std::cout << "mouse event 1" << endl;
	}
	else if (mMouseState.buttons & 2)
	{
		//fire mouse event with left click and mouse position
		mpEventSystem->fireEvent(MouseEvent(2, mMouseState.x, mMouseState.y));
		std::cout << MouseEvent(2, mMouseState.x, mMouseState.y).getType() << endl;
	}

	al_get_keyboard_state(&mKeyState);//get allegro key state

	if (al_key_down(&mKeyState, ALLEGRO_KEY_ENTER))
	{
		//fire key event with enter click
		mpEventSystem->fireEvent(KeyEvent("enter"));
		std::cout << "key event enter" << endl;
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_ESCAPE))
	{
		//fire key event with esc click
		mpEventSystem->fireEvent(KeyEvent("esc"));
		std::cout << "key event esc" << endl;
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_SPACE))
	{
		//fire key event with space click
		mpEventSystem->fireEvent(KeyEvent("space"));
		std::cout << "key event space" << endl;
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_1))
	{
		//fire key event with space click
		mpEventSystem->fireEvent(KeyEvent("1"));
		std::cout << "key event 1" << endl;
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_2))
	{
		//fire key event with space click
		mpEventSystem->fireEvent(KeyEvent("2"));
		std::cout << "key event 2" << endl;
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_3))
	{
		//fire key event with space click
		mpEventSystem->fireEvent(KeyEvent("3"));
		std::cout << "key event 3" << endl;
	}
}
