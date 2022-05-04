#include "ExampleListener.h"
#include "EventSystem.h"
#include "Event.h"
#include "MoveEvent.h"
#include "StringEvent.h"

ExampleListener::ExampleListener()
{
	EventSystem* pEventSystem = EventSystem::getInstance();
	pEventSystem->addListener( MOVE_EVENT, this );
	pEventSystem->addListener(MESSAGE_EVENT, this);
}

ExampleListener::~ExampleListener()
{
}

void ExampleListener::handleEvent(const Event &theEvent)
{
	cout << endl << "ExampleListener" << endl;
	
	if( theEvent.getType() == MOVE_EVENT )
	{
		const MoveEvent& moveEvent = static_cast<const MoveEvent&>(theEvent);
		cout	<< endl << "\tMove to: " << moveEvent.getLocation().getX()
				<< "," << moveEvent.getLocation().getY() << endl;
	}
	else if( theEvent.getType() == MESSAGE_EVENT)
	{
		const StringEvent& stringEvent = static_cast<const StringEvent&>(theEvent);
		cout	<< endl << "\tMessage: " << stringEvent.getString() << endl;
	}
}