#pragma once

#include "Event.h"

enum ButtonState;

class MouseEvent : public Event
{

public:
    MouseEvent(int button, ButtonState);
    ~MouseEvent();

    int getMouseButton() const { return mButton; }
    int getButtonState() const { return mState; }

private:
    MouseEvent() = delete;

    int mButton;
    ButtonState mState;

};