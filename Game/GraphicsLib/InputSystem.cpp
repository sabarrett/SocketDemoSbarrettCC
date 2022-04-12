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
	coolDown = 0;
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
	coolDown++;

	//std::cout << coolDown << endl;
	int eee = 10;

	al_get_keyboard_state(&mKeyState);//get allegro key state

	if (al_key_down(&mKeyState, ALLEGRO_KEY_DELETE) || al_key_down(&mKeyState, ALLEGRO_KEY_BACKSPACE))
	{
		//fire key event with enter click
		if (coolDown > eee)
		{
			mpEventSystem->fireEvent(KeyEvent("enter"));
			coolDown = 0;
		}
		//std::cout << "key event enter" << endl;
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_ESCAPE))
	{
		if (coolDown > eee)
		{
		//fire key event with esc click
		mpEventSystem->fireEvent(KeyEvent("esc"));
		//std::cout << "key event esc" << endl;
		coolDown = 0;
		}
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_1))
	{
				if (coolDown > eee)
				{
		//fire key event with space click
		mpEventSystem->fireEvent(KeyEvent("1"));
		//std::cout << "key event 1" << endl;
		coolDown = 0;
				}
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_2))
	{
					if (coolDown > eee)
					{
		//fire key event with space click
		mpEventSystem->fireEvent(KeyEvent("2"));
		//std::cout << "key event 2" << endl;
		coolDown = 0;
					}
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_3))
	{
						if (coolDown > eee)
						{
		//fire key event with space click
		mpEventSystem->fireEvent(KeyEvent("3"));
		//std::cout << "key event 3" << endl;
		coolDown = 0;
						}
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_W))
	{
		if (coolDown > eee)
		{
			//fire key event with space click
			mpEventSystem->fireEvent(KeyEvent("W"));
			//std::cout << "key event 3" << endl;
			coolDown = 0;
		}
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_A))
	{
		if (coolDown > eee)
		{
			//fire key event with space click
			mpEventSystem->fireEvent(KeyEvent("A"));
			//std::cout << "key event 3" << endl;
			coolDown = 0;
		}
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_S))
	{
		if (coolDown > eee)
		{
			//fire key event with space click
			mpEventSystem->fireEvent(KeyEvent("S"));
			//std::cout << "key event 3" << endl;
			coolDown = 0;
		}
	}
	else if (al_key_down(&mKeyState, ALLEGRO_KEY_D))
	{
		if (coolDown > eee)
		{
			//fire key event with space click
			mpEventSystem->fireEvent(KeyEvent("D"));
			//std::cout << "key event 3" << endl;
			coolDown = 0;
		}
	}
}
