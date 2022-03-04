/*******************************************************************************************
*
*   raylib-cpp [core] example - Basic window
*
*   Welcome to raylib-cpp!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib-cpp. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib-cpp is licensed under an unmodified zlib/libpng license (View raylib-cpp.hpp for details)
*
*   Copyright (c) 2021 Rob Loach (@RobLoach)
*
********************************************************************************************/

#include "Game.h"

#include "MemoryTracker.h"
#include "Timer.h"

#include <iostream>

const int screenWidth = 800;
const int screenHeight = 450;

int main()
{

    //Initialize runtime timer
    Timer t;
    t.start();
    
    //Play Game
    Game::getInstance()->init(screenWidth, screenHeight, 60, true);
    Game::getInstance()->startGame();
    Game::getInstance()->cleanup();
    Game::cleanupInstance();

    //Track Memory Leaks
    MemoryTracker::cleanupInstance();

    //Output runtime
    std::cout << "----------Total runtime: " << t.getElapsedTime() << std::endl;

    std::string temp;
    std::cin >> temp;

    return 0;
}
