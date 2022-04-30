#pragma once
using namespace std;

#include <string>

#include "GraphicsSystem.h"
#include "InputSystem.h"



class System 
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