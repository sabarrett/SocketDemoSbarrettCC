
#include <iostream>
#include <string>

#include <Trackable.h>
#include <MemoryTracker.h>

#include <Timer.h>
#include <PerformanceTracker.h>
#include <Vector2D.h>

#include "EventSystem.h"
#include "ExampleListener.h"
#include "MoveEvent.h"
#include "StringEvent.h"
#include "SoundSystem.h"

int main()
{
	EventSystem::initInstance();
	EventSystem* pEventSystem = EventSystem::getInstance();

	pEventSystem->init();

	ExampleListener* pListener = new ExampleListener;
	SoundSystem* pSoundSystem = new SoundSystem;


	//fire some events
	pEventSystem->fireEvent( MoveEvent( Vector2D( 1.0f, 2.0f ) ) );

	pEventSystem->fireEvent( StringEvent( "Initialization complete" ) );

	pEventSystem->fireEvent( StringEvent( "Text1" ) );


	delete pListener;//make sure you delete listeners before the event system they use

	pEventSystem->fireEvent(StringEvent("Text\n"));

	delete pSoundSystem;

	EventSystem::cleanupInstance();
	MemoryTracker::getInstance()->reportAllocations(std::cout);

	system("pause");

	return 0;
}