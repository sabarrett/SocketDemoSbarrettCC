#pragma once

#include <map>
#include <Trackable.h>

/*
class GameEvent;
class Game;
enum GameEventType;

using namespace std;

class GameEventSystem : public Trackable
{
public:
	static GameEventSystem* getInstance();
	static void initInstance();
	static void cleanupInstance();

	void init();
	void cleanup();

	void fireEvent(const GameEvent& theEvent);
	void addListener(GameEventType type, Game* pListener);
	void removeListener(GameEventType type, Game* pListener);
	void removeListenerFromAllEvents(Game* pListener);

	GameEventSystem();
	~GameEventSystem();

private:
	static GameEventSystem* mspInstance;
	multimap< GameEventType, Game* > mListenerMap;
	bool mIsInitted = false;

	void dispatchAllEvents(const GameEvent& theEvent);
};*/