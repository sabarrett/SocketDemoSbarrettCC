#pragma once
using namespace std;

#include <string>

#include "GraphicsSystem.h"
#include "InputSystem.h"

#include "Trackable.h"

class System : public Trackable
{
private:
    GraphicsSystem * mpGraphicsSystem;
    InputSystem* mpISystem;

public:
    System();
    ~System();

    void init(int width, int height);
    void cleanup();

    void update();

    friend class Game;
};