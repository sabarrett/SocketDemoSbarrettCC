#include "SoundSystem.h"
#include "EventSystem.h"
#include "Event.h"

SoundSystem::SoundSystem()
{
	EventSystem::getInstance()->addListener(MESSAGE_EVENT, this);
}

void SoundSystem::handleEvent(const Event& theEvent)
{
	cout << endl << "SoundSystem" << endl;

	if (theEvent.getType() == MESSAGE_EVENT)
	{
		cout << endl << "Ding" << endl;
	}
}
