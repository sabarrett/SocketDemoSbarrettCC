#include "InputSystem.h"
#include "MouseEvent.h"

MouseEvent::MouseEvent(int button, ButtonState state)
    : Event(MOUSE_EVENT)
{
    mButton = button;
    mState = state;
}

MouseEvent::~MouseEvent()
{

}