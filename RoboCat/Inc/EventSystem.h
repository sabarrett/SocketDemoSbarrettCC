#pragma once

#include <map>

class Event;
class EventListener;
enum EventType;

class EventSystem
{
public:

	static EventSystem* getInstance();
	static void initInstance();
	static void cleanupInstance();

	void playEvent(const Event& theEvent);
	void addListener(EventType type, EventListener* pListener);
	void removeListener(EventType type, EventListener* pListener);
	void removeListenerFromAllEvents(EventListener* pListener);

private:
	static EventSystem* mspInstance;
	std::multimap<EventType, EventListener*> mListenMap;
	bool mIsInit = false;

	EventSystem();
	~EventSystem();
};