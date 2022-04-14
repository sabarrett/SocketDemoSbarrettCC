#include "InputSystem.h"
//#include "RoboCatPCH.h"

InputSystem::InputSystem()
{

	mpKeyboardState = new ALLEGRO_KEYBOARD_STATE();

	mpKeyboardEvents = al_create_event_queue();
}

InputSystem::~InputSystem()
{
	cleanup();
}

void InputSystem::cleanup()
{
	delete mpKeyboardState;
	mpKeyboardState = nullptr;

	al_unregister_event_source(mpKeyboardEvents, al_get_keyboard_event_source());
	delete mpKeyboardEvents;
	mpKeyboardEvents = nullptr;

	al_uninstall_keyboard();
}

bool InputSystem::initInputSystem(GraphicsSystems* mpGraphicsSystem)
{
	if (!al_install_keyboard())
	{
		//std::cout << "error initting keyboard\n";
		return false;
	}

	al_register_event_source(mpKeyboardEvents, al_get_display_event_source(mpGraphicsSystem->getmpDisplay()));
	al_register_event_source(mpKeyboardEvents, al_get_keyboard_event_source());

	return true;
}

void InputSystem::keyUpdate()//updates the key state
{
	al_get_keyboard_state(mpKeyboardState);
}


bool InputSystem::getKeyState(int keycode)//tracks the current key stokes
{
	keyUpdate();
	return al_key_down(mpKeyboardState, keycode);
}