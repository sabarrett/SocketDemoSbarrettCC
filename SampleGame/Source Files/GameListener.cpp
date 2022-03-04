#include "GameListener.h"
#include "PlayerMoveEvent.h"
#include "KeyboardEvent.h"
#include "MouseEvent.h"
#include "Game.h"
#include "GraphicsSystem.h"

GameListener::GameListener()
{

}

GameListener::~GameListener()
{

}

void GameListener::handleEvent(const Event& event)
{
    if(event.getType() == PLAYER_MOVE_EVENT)
    {
        const PlayerMoveEvent& moveEvent = (const PlayerMoveEvent&)event;

        Game::getInstance()->DPlayerMove(moveEvent.getMoveLoc());
    }
    else if(event.getType() == KEYBOARD_EVENT)
    {
        const KeyboardEvent& keyboardEvent = (const KeyboardEvent&)event;

        if(keyboardEvent.getButtonState() == BUTTON_DOWN)
        {
	        processKeyDown(keyboardEvent.getKey());
        }
        else if(keyboardEvent.getButtonState() == BUTTON_HELD)
        {
            processKey(keyboardEvent.getKey());
        }
        else if(keyboardEvent.getButtonState() == BUTTON_UP)
        {
            if(keyboardEvent.getKey() == Key_A || keyboardEvent.getKey() == Key_D)
	        {
                InputSystem::getInstance()->setHorizonalMovementAxis(0.0f);
	        }
	        if(keyboardEvent.getKey() == Key_S || keyboardEvent.getKey() == Key_W)
	        {
		        InputSystem::getInstance()->setVerticalMovementAxis(0.0f);
	        }

            //Game::getInstance()->DKeyRelease(keyboardEvent.getKey());
        }
    }
    else if(event.getType() == MOUSE_EVENT)
    {
        const MouseEvent& mouseEvent = (const MouseEvent&)event;

        if(mouseEvent.getButtonState() == BUTTON_DOWN)
        {

            if(mouseEvent.getMouseButton() == 0)
	        {
		        Game::getInstance()->fireProj();
	        }

            //Game::getInstance()->DMousePress(mouseEvent.getMouseButton());
        }
        else if(mouseEvent.getButtonState() == BUTTON_UP)
        {
            //Game::getInstance()->DMouseRelease(mouseEvent.getMouseButton());
        }
    }
}

void GameListener::processKeyDown(KeyCode key)
{

    switch(key)
    {
        case Key_Escape:
            Game::getInstance()->quitGame();
            break;
    }

    //Game::getInstance()->DKeyPress(keyboardEvent.getKey());
}

void GameListener::processKey(KeyCode key)
{

    Vector2D loc;
    switch(key)
    {
        case Key_Right:
            loc = Game::getInstance()->mpGraphicsSystem->getCameraLocation();
            Game::getInstance()->mpGraphicsSystem->setCameraLocation(loc + Vector2D(20, 0));
            break;

        case Key_Left:
            loc = Game::getInstance()->mpGraphicsSystem->getCameraLocation();
            Game::getInstance()->mpGraphicsSystem->setCameraLocation(loc + Vector2D(-20, 0));
            break;

        case Key_Down:
            loc = Game::getInstance()->mpGraphicsSystem->getCameraLocation();
            Game::getInstance()->mpGraphicsSystem->setCameraLocation(loc + Vector2D(0, 20));
            break;

        case Key_Up:
            loc = Game::getInstance()->mpGraphicsSystem->getCameraLocation();
            Game::getInstance()->mpGraphicsSystem->setCameraLocation(loc + Vector2D(0, -20));
            break;

        case Key_A:
            InputSystem::getInstance()->setHorizonalMovementAxis(Vector2D::Left().getX());
            break;

        case Key_S:
            InputSystem::getInstance()->setVerticalMovementAxis(Vector2D::Down().getY());
            break;

        case Key_D:
            InputSystem::getInstance()->setHorizonalMovementAxis(Vector2D::Right().getX());
            break;

        case Key_W:
            InputSystem::getInstance()->setVerticalMovementAxis(Vector2D::Up().getY());
            break;
    }

}