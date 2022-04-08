#pragma once

#include "Trackable.h"
#include "Event.h"
#include <map>

class EventListener;

class EventSystem : public Trackable
{

public:
    static void cleanupInstance();
    static EventSystem* getInstance();

    void addListener(EventType, EventListener*);
    bool removeListener(EventType, EventListener*);
    void removeListenerFromAllEvents(EventListener*);
    void fireEvent(const Event&);

private:
    EventSystem();
    ~EventSystem();

    static EventSystem* mspInstance;
    std::multimap<EventType, EventListener*> mListenerMap;

};