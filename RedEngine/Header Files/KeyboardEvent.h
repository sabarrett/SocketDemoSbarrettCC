#pragma once

#include "Event.h"
#include "InputSystem.h"

class KeyboardEvent : public Event
{

public:
    KeyboardEvent(KeyCode, ButtonState);
    ~KeyboardEvent();

    KeyCode getKey() const { return mKey; }
    ButtonState getButtonState() const { return mState; }

private:
    KeyboardEvent() = delete;

    KeyCode mKey;
    ButtonState mState;

};