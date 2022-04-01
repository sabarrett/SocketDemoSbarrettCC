#include "GameController.h"

GameController::GameController()
{
	p1Score = 0;
	p2Score = 0;
}

GameController::GameController(Colour p1, Colour p2)
{
	p1Color = p1;
	p2Color = p2;
}

void GameController::dropBoulder()
{
	//spawn a boulder at random location on top of the screen
	//move from top to bottom
	//destroy /any/ bubbles it touches on the way
}

void GameController::sendBees(KeyCode direction)
{
	//spawn a horde of bees at opposite side of screen
	//bees move across screen in designated direction
	//destroy /any/ bubbles they touch
}

void GameController::makeBubble(std::string playerID)
{
	Colour bubbleColor;
	if (playerID == player1ID)
	{
		bubbleColor = p1Color;
	}

	else
	{
		bubbleColor = p2Color;
	}
}

void GameController::sendDart()
{
	//destroy a random bubble
}