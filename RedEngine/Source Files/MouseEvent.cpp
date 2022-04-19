#include "InputSystem.h"
#include "MouseEvent.h"

MouseEvent::MouseEvent(int button, ButtonState state, float time)
    : Event(MOUSE_ACTION_EVENT)
{
    mButton = button;
    mState = state;
    this->time = time;
}

MouseEvent::~MouseEvent()
{

}