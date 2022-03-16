#include"GameEvent.h"

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