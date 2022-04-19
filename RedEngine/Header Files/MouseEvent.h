#pragma once

#include "Event.h"

enum ButtonState;

class MouseEvent : public Event
{

public:
    MouseEvent(int button, ButtonState, float);
    ~MouseEvent();

    int getMouseButton() const { return mButton; }
    int getButtonState() const { return mState; }
    float getTime() const { return time; }

private:
    MouseEvent() = delete;

    int mButton;
    ButtonState mState;
    float time;

};