#pragma once
#include <string>
#include "Colour.h"
#include "InputSystem.h"

//makeshift game manager
class GameController
{
public:

	GameController();
	GameController(Colour p1, Colour p2);
	~GameController() {};

	void dropBoulder();
	void sendBees(KeyCode direction);
	void makeBubble(std::string playerID);
	void sendDart();

private:

	std::string player1ID;
	std::string player2ID;

	Colour p1Color;
	Colour p2Color;

	int p1Score;
	int p2Score;
};