#pragma once

#include "EventListener.h"
#include "EventSystem.h"
#include "Game.h"

#include "DeleteUnitsEvent.h"
#include "PlaceUnitEvent.h"
#include "EscEvent.h"
#include "PauseEvent.h"
#include "SwitchAnimEvent.h"
#include "WASDEvent.h"

using namespace std;

//A listener specifically for game events
class GameListener : public EventListener
{
public:
	GameListener();
	~GameListener();

	void init();
	void cleanup();

	void handleEvent(const Event& theEvent);

private:
	Game* mpGame;
};