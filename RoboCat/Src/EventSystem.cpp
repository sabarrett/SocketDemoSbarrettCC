#include "EventSystem.h"
#include "Event.h"
#include "EventListener.h"
#include <cassert>

EventSystem* EventSystem::mspInstance = nullptr;

void EventSystem::addListener(EventType type, EventListener* pListener)
{
	mListenMap.insert(std::pair<EventType, EventListener*>(type, pListener));
}

void EventSystem::removeListener(EventType type, EventListener* pListener)
{
	if (mIsInit)
	{
		std::pair<std::multimap<EventType, EventListener*>::iterator, std::multimap<EventType, EventListener*>::iterator> ret;

		ret = mListenMap.equal_range(type);
		std::multimap<EventType, EventListener*>::iterator iter;

		for (iter = ret.first; iter != ret.second; iter++)
		{
			if (iter->second == pListener)
			{
				mListenMap.erase(iter);
				break;
			}
		}
	}
}

void EventSystem::removeListenerFromAllEvents(EventListener* pListener)
{
	if (mIsInit)
	{
		std::multimap<EventType, EventListener*>::iterator iter;

		bool complete = false;

		while (!complete)
		{
			complete = true; //don't like this
			for (iter = mListenMap.begin(); iter != mListenMap.end(); iter++)
			{
				if (iter->second == pListener)
				{
					mListenMap.erase(iter);
					complete = false;
					break;
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
	mspInstance = new EventSystem();
}

void EventSystem::cleanupInstance()
{
	delete mspInstance;
	mspInstance = nullptr;
}

void EventSystem::playEvent(const Event& theEvent)
{
	std::pair<std::multimap<EventType, EventListener*>::iterator, std::multimap<EventType, EventListener*>::iterator> ret;
	ret = mListenMap.equal_range(theEvent.getType());

	std::multimap<EventType, EventListener*>::iterator iter;
	for (iter = ret.first; iter != ret.second; ++iter)
	{
		iter->second->handleEvent(theEvent);
	}
}