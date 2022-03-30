#include "EventSystem.h"
#include "Event.h"
#include "EventListener.h"
#include <cassert>

EventSystem* EventSystem::mspInstance = nullptr;

EventSystem::EventSystem()
{
	mIsInitted = true;
}

EventSystem::~EventSystem()
{
	cleanup();
}

void EventSystem::addListener(EventType type, EventListener* pListener)
{

	if (mIsInitted)
	{
		std::cout << "adding a listner " << type << endl;
		mListenerMap.insert(pair< EventType, EventListener* >(type, pListener));
	}
}

void EventSystem::removeListener(EventType type, EventListener *pListener)
{
	if (mIsInitted)
	{
		pair<multimap<EventType, EventListener*>::iterator, multimap<EventType, EventListener*>::iterator> ret;

		ret = mListenerMap.equal_range(type);
		multimap<EventType, EventListener*>::iterator iter;

		for (iter = ret.first; iter != ret.second; ++iter)
		{
			if (iter->second == pListener)
			{
				mListenerMap.erase(iter);
				break;//to prevent using invalidated iterator
			}
		}
	}
}

void EventSystem::removeListenerFromAllEvents( EventListener* pListener )
{
	if (mIsInitted)
	{
		multimap<EventType, EventListener*>::iterator iter;

		bool allTheWayThrough = false;

		while (!allTheWayThrough)
		{
			allTheWayThrough = true;
			for (iter = mListenerMap.begin(); iter != mListenerMap.end(); ++iter)
			{
				if (iter->second == pListener)
				{
					mListenerMap.erase(iter);
					allTheWayThrough = false; //didn't make it the whole way through
					break;//to prevent using invalidated iterator
				}
			}
		}
	}
}

EventSystem* EventSystem::getInstance()
{
	assert(mspInstance);
	return mspInstance;
}

void EventSystem::initInstance()
{
	assert(!mspInstance);
	mspInstance = new EventSystem;
}

void EventSystem::cleanupInstance()
{
	delete mspInstance;
	mspInstance = nullptr;
}

void EventSystem::init()
{
	if (mIsInitted)
	{
		cleanup();
	}
	mIsInitted = true;
}

void EventSystem::cleanup()
{
	mListenerMap.clear();
	mIsInitted = false;
}

void EventSystem::fireEvent( const Event& theEvent )
{
	std::cout << "firing event" << theEvent.getType() << endl;

	if (mIsInitted)
	{
		dispatchAllEvents(theEvent);
	}
}

void EventSystem::dispatchAllEvents( const Event& theEvent )
{
	if (mIsInitted)
	{
		std::cout << "dispatching event" << endl;
		pair<multimap<EventType, EventListener*>::iterator, multimap<EventType, EventListener*>::iterator> ret;
		ret = mListenerMap.equal_range(KEY_EVENT_B);

		multimap<EventType, EventListener*>::iterator iter;
		for (iter = ret.first; iter != ret.second; ++iter)
		{
			std::cout << "sending to listener" << endl;
			iter->second->handleEvent(theEvent);
		}
	}
}