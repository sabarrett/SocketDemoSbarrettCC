#include"GameEvent.h"

/*
Author: Nicholas Preis
	Class : Game Architecture <250-71>
	Assignment : Assignment 5
	Certification of Authenticity :
I certify that this assignment is entirely my own work.
*/

GameEvent::GameEvent(GameEventType type) : Event(EventType(type))
{
	mType = type;
	mPosX = 0;
	mPosY = 0;
}

GameEvent::GameEvent(GameEventType type, int posX, int posY) : Event(EventType(type))
{
	mType = type;
	mPosX = posX;
	mPosY = posY;
}

GameEvent::~GameEvent()
{
}