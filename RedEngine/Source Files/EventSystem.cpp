#include "Event.h"
#include "EventListener.h"
#include "EventSystem.h"

using namespace std;

EventSystem* EventSystem::mspInstance = nullptr;

EventSystem* EventSystem::getInstance()
{
    if(!mspInstance)
        mspInstance = new EventSystem();
        
    return mspInstance;
}

void EventSystem::cleanupInstance()
{
    if(mspInstance)
        delete mspInstance;
}

void EventSystem::addListener(EventType type, EventListener* listener)
{
    mListenerMap.insert(pair<EventType, EventListener*>(type, listener));
}

bool EventSystem::removeListener(EventType type, EventListener* listener)
{
    pair<multimap<EventType, EventListener*>::iterator, multimap<EventType, EventListener*>::iterator> range;
    range = mListenerMap.equal_range(type);

    for(multimap<EventType, EventListener*>::iterator i = range.first; i!= range.second; ++i)
    {
        if(i->second == listener)
        {
            mListenerMap.erase(i);
            return true;
        }
    }
    return false;
}

void EventSystem::removeListenerFromAllEvents(EventListener* listener)
{

    bool allTheWayThrough = false;

    while(!allTheWayThrough)
    {
        allTheWayThrough = true;
        for(multimap<EventType, EventListener*>::iterator i = mListenerMap.begin(); i != mListenerMap.end(); ++i)
        {
            if(i->second == listener)
            {
                mListenerMap.erase(i);
                allTheWayThrough = false;
                break;
            }
        }
    }
    
}

void EventSystem::fireEvent(const Event& event)
{
    pair<multimap<EventType, EventListener*>::iterator, multimap<EventType, EventListener*>::iterator> range;
    range = mListenerMap.equal_range(event.getType());

    for(multimap<EventType, EventListener*>::iterator i = range.first; i != range.second; ++i)
    {
        i->second->handleEvent(event);
    }
}

EventSystem::EventSystem()
{

}

EventSystem::~EventSystem()
{

}