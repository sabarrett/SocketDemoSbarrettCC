#pragma once

#include "EventListener.h"
#include "InputSystem.h"


class GameListener : public EventListener
{

public:
    GameListener();
    ~GameListener();

    void handleEvent(const Event&);

    void processKeyUp(KeyCode key);
    void processKey(KeyCode key);
    void processKeyDown(KeyCode key);

    void processMouseButtonUp(int button);
    void processMouseButton(int button);
    void processMouseButtonDown(int button);

};