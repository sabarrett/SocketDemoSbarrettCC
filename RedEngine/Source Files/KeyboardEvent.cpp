#include "InputSystem.h"
#include "KeyboardEvent.h"

KeyboardEvent::KeyboardEvent(KeyCode key, ButtonState state)
    : Event(KEYBOARD_EVENT)
{
    mKey = key;
    mState = state;
}

KeyboardEvent::~KeyboardEvent()
{

}