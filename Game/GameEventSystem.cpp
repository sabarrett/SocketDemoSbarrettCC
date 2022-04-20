#include "GameEventSystem.h"
#include "GameEvent.h"
#include "Game.h"
#include <cassert>

/*GameEventSystem* GameEventSystem::mspInstance = nullptr;

GameEventSystem::GameEventSystem()
{
	mIsInitted = true;
}

GameEventSystem::~GameEventSystem()
{
	cleanup();
}

void GameEventSystem::addListener(GameEventType type, Game* pListener)
{
	if (mIsInitted)
	{
		mListenerMap.insert(pair< GameEventType, Game* >(type, pListener));
	}
}

void GameEventSystem::removeListener(GameEventType type, Game* pListener)
{
	if (mIsInitted)
	{
		pair<multimap<GameEventType, Game*>::iterator, multimap<GameEventType, Game*>::iterator> ret;

		ret = mListenerMap.equal_range(type);
		multimap<GameEventType, Game*>::iterator iter;

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

void GameEventSystem::removeListenerFromAllEvents(Game* pListener)
{
	if (mIsInitted)
	{
		multimap<GameEventType, Game*>::iterator iter;

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

GameEventSystem* GameEventSystem::getInstance()
{
	assert(mspInstance);
	return mspInstance;
}

void GameEventSystem::initInstance()
{
	assert(!mspInstance);
	mspInstance = new GameEventSystem;
}

void GameEventSystem::cleanupInstance()
{
	delete mspInstance;
	mspInstance = nullptr;
}

void GameEventSystem::init()
{
	if (mIsInitted)
	{
		cleanup();
	}
	mIsInitted = true;
}

void GameEventSystem::cleanup()
{
	mListenerMap.clear();
	mIsInitted = false;
}

void GameEventSystem::fireEvent(const GameEvent& theEvent)
{
	if (mIsInitted)
	{
		dispatchAllEvents(theEvent);
	}
}

void GameEventSystem::dispatchAllEvents(const Event& theEvent)
{
	if (mIsInitted)
	{
		pair<multimap<GameEventType, Game*>::iterator, multimap<GameEventType, Game*>::iterator> ret;
		ret = mListenerMap.equal_range(theEvent.getType());

		multimap<GameEventType, Game*>::iterator iter;
		for (iter = ret.first; iter != ret.second; ++iter)
		{
			iter->second->handleEvent(theEvent);
		}
	}
}*/