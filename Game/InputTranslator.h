#pragma once

#include <string>
#include "EventListener.h"
#include "EventSystem.h"

#include "DeleteUnitsEvent.h"
#include "PlaceUnitEvent.h"
#include "EscEvent.h"
#include "PauseEvent.h"
#include "SwitchAnimEvent.h"
#include "KeyEvent.h"
#include "MouseEvent.h"
#include "WASDEvent.h"

using namespace std;

class InputTranslator : public EventListener
{
public:
	InputTranslator();
	~InputTranslator();

	void init();
	void cleanup();

	void handleEvent(const Event& theEvent);

private:
	EventSystem* mpESystem;
};