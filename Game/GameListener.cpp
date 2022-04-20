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
	if (theEvent.getType() == 5)
	{
		//std::cout << "escape game" << endl;
		mpGame->escape();
		mpGame->setWorldStateChanged(true);
	}
	//pause game
	else if (theEvent.getType() == 8)
	{
		mpGame->pause();
		mpGame->setWorldStateChanged(true);
	}
	//switch animations
	else if (theEvent.getType() == SWITCH_ANIM_EVENT)
	{
		mpGame->switchAnim();
		mpGame->setWorldStateChanged(true);
	}
	//place unit
	else if (theEvent.getType() == 6)
	{
		const PlaceUnitEvent& placeUnitEvent = static_cast<const PlaceUnitEvent&>(theEvent);
		int x = 50;
		int y = 50;

		mpGame->placeUnit(placeUnitEvent.getX());
		mpGame->setWorldStateChanged(true);
	//	cout << "setting to true b";
	}
	//delete units
	else if (theEvent.getType() == 7)
	{

		std::cout << "I am deleteing a unity :)" << endl;
		const DeleteUnitsEvent& deleteUnitsEvent = static_cast<const DeleteUnitsEvent&>(theEvent);

		//erase all units in a vicinity
		mpGame->deleteUnits();
		mpGame->setWorldStateChanged(true);
	}

	
}

