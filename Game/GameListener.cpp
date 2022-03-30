#include "GameListener.h"

GameListener::GameListener()
{
}

GameListener::~GameListener()
{
}

void GameListener::init()
{
	//Establish Game listener as a listener to events
	EventSystem* pEventSytem = EventSystem::getInstance();
	pEventSytem->addListener(DELETE_UNITS_EVENT, this);
	pEventSytem->addListener(PLACE_UNIT_EVENT, this);
	pEventSytem->addListener(PAUSE_EVENT, this);
	pEventSytem->addListener(ESC_EVENT, this);
	pEventSytem->addListener(SWITCH_ANIM_EVENT, this);

	mpGame = Game::getInstance();
}

void GameListener::cleanup()
{
	EventSystem::getInstance()->removeListenerFromAllEvents(this);
}

void GameListener::handleEvent(const Event& theEvent)
{
	//escape game
	if (theEvent.getType() == ESC_EVENT)
	{
		mpGame->escape();
	}
	//pause game
	else if (theEvent.getType() == PAUSE_EVENT)
	{
		mpGame->pause();
	}
	//switch animations
	else if (theEvent.getType() == SWITCH_ANIM_EVENT)
	{
		mpGame->switchAnim();
	}
	//place unit
	else if (theEvent.getType() == PLACE_UNIT_EVENT)
	{
		const PlaceUnitEvent& placeUnitEvent = static_cast<const PlaceUnitEvent&>(theEvent);
		int x = placeUnitEvent.getX();
		int y = placeUnitEvent.getY();

		mpGame->placeUnit(x, y);
	}
	//delete units
	else if (theEvent.getType() == DELETE_UNITS_EVENT)
	{
		const DeleteUnitsEvent& deleteUnitsEvent = static_cast<const DeleteUnitsEvent&>(theEvent);
		int x = deleteUnitsEvent.getX();
		int y = deleteUnitsEvent.getY();

		//erase all units in a vicinity
		mpGame->deleteUnits(x, y);
	}
}

