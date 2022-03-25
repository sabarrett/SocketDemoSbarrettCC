#include "System.h"
#include "GraphicsSystem.h"

#include <iostream>

using namespace std;

System::System()
{
	mpGraphicsSystem = new GraphicsSystem;
}

System::~System()
{
	cleanup();
	delete mpGraphicsSystem;
}

bool System::init(unsigned int width, unsigned int height)
{
	if (!mIsInitted)
	{
		if (!al_init())
		{
			cout << "error initting Allegro\n";
			return false;
		}
		else
		{
			bool ret = mpGraphicsSystem->init(width, height);
			if (!ret)
			{
				cout << "error initting GraphicsSystem\n";
				return false;
			}
		}
		mIsInitted = true;
	}
	return true;
}

void System::cleanup()
{
	if (mIsInitted)
	{
		mpGraphicsSystem->cleanup();
		mIsInitted = false;
	}
}

Vector2D System::getCurrentMousePos()
{
	ALLEGRO_MOUSE_STATE state;

	al_get_mouse_state(&state);

	return Vector2D(state.x, state.y);
}

bool System::isMouseButtonPressed(System::MouseButton button)
{
	ALLEGRO_MOUSE_STATE state;

	al_get_mouse_state(&state);

	if (button == LEFT)
	{
		if (state.buttons & 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (button == RIGHT)
	{
		if (state.buttons & 2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (button == CENTER)
	{
		if (state.buttons & 4)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool System::isKeyPressed(Key theKey)
{
	ALLEGRO_KEYBOARD_STATE state;
	al_get_keyboard_state(&state);

	if (al_key_down(&state, theKey))
	{
		return true;
	}

	return false;
}

