#pragma once
#include<iostream>
#include<allegro5/allegro.h>
#include<allegro5/keyboard.h>
#include<allegro5/mouse.h>
#include"../common/DeanLib/include/Trackable.h"

using namespace std;

class Event;
class EventSystem;

class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	void init();
	void cleanup();
	void update();

	// This enum is all of the keycodes used by the input system. I highly recommend collapsing this in VS.
// It should also be noted that these are mapped identically to the Allegro keycodes and can actually replace them in code - wes
	enum Keycode
	{
		KEY_A = 1,
		KEY_B,
		KEY_C,
		KEY_D,
		KEY_E,
		KEY_F,
		KEY_G,
		KEY_H,
		KEY_I,
		KEY_J,
		KEY_K,
		KEY_L,
		KEY_M,
		KEY_N,
		KEY_O,
		KEY_P,
		KEY_Q,
		KEY_R,
		KEY_S,
		KEY_T,
		KEY_U,
		KEY_V,
		KEY_W,
		KEY_X,
		KEY_Y,
		KEY_Z,
		KEY_0,
		KEY_1,
		KEY_2,
		KEY_3,
		KEY_4,
		KEY_5,
		KEY_6,
		KEY_7,
		KEY_8,
		KEY_9,
		KEY_PAD_0,
		KEY_PAD_1,
		KEY_PAD_2,
		KEY_PAD_3,
		KEY_PAD_4,
		KEY_PAD_5,
		KEY_PAD_6,
		KEY_PAD_7,
		KEY_PAD_8,
		KEY_PAD_9,
		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,
		KEY_ESC,
		KEY_TILDE,
		KEY_MINUS,
		KEY_EQUALS,
		KEY_BACKSPACE,
		KEY_TAB,
		KEY_OPENBRACE,
		KEY_CLOSEBRACE,
		KEY_ENTER,
		KEY_SEMICOLON,
		KEY_QUOTE,
		KEY_BACKSLASH,
		KEY_BACKSLASH2,
		KEY_COMMA,
		KEY_FULLSTOP,
		KEY_SLASH,
		KEY_SPACE,
		KEY_INSERT,
		KEY_DELETE,
		KEY_HOME,
		KEY_END,
		KEY_PGUP,
		KEY_PGDN,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_UP,
		KEY_DOWN,
		KEY_PAD_SLASH,
		KEY_PAD_ASTERISK,
		KEY_PAD_MINUS,
		KEY_PAD_PLUS,
		KEY_PAD_DELETE,
		KEY_PAD_ENTER,
		KEY_PRINTSCREEN,
		KEY_PAUSE,
		KEY_ABNT_C1,
		KEY_YEN,
		KEY_KANA,
		KEY_CONVERT,
		KEY_NOCONVERT,
		KEY_AT,
		KEY_CIRCUMFLEX,
		KEY_COLON2,
		KEY_KANJI,
		KEY_LSHIFT,
		KEY_RSHIFT,
		KEY_LCTRL,
		KEY_RCTRL,
		KEY_ALT,
		KEY_ALTGR,
		KEY_LWIN,
		KEY_RWIN,
		KEY_MENU,
		KEY_SCROLLLOCK,
		KEY_NUMLOCK,
		KEY_CAPSLOCK,
		KEY_PAD_EQUALS,
		KEY_BACKQUOTE,
		KEY_SEMICOLON2,
		KEY_COMMAND,
	};

	// This is the same as Keycode but specifically for mouse buttons
	enum Mousecode
	{
		MOUSE_1 = 1,
		MOUSE_2,
		MOUSE_3,
		MOUSE_4,
	};

private:
	ALLEGRO_EVENT_QUEUE* mpEventQueue;

	int mPosX;
	int mPosY;

	bool mDeleted;
};

