#pragma once
#include<iostream>
#include"Event.h"

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 5
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

using namespace std;

enum GameEventType
{
	INVALID_TYPE_OF_EVENT = 7,
	CREATE_UNIT_EVENT,
	DESTROY_UNIT_EVENT,
	UPDATE_MOUSE_POSITION_EVENT,
	CHANGE_UNIT_ANIMATION_EVENT,
	TOGGLE_ALL_UNIT_ANIMATIONS_EVENT,
	EXIT_GAME_EVENT,
	NUM_OF_EVENT_TYPES
};

class GameEvent : public Event
{
public:
	GameEvent(GameEventType type);
	GameEvent(GameEventType type, int posX, int posY);
	~GameEvent();

	int getPosX() const { return mPosX; }
	int getPosY() const { return mPosY; }

	GameEventType getType() const { return mType; }
private:
	int mPosX;
	int mPosY;

	GameEventType mType;
};

