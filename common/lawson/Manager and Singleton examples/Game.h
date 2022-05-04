#pragma once

#include <assert.h>

class Game
{
public:
	static Game* getInstance(){ assert(mpGameInstance != nullptr); return mpGameInstance; };
	static void initInstance(){ mpGameInstance = new Game; };
	static void cleanupInstance(){ delete mpGameInstance; };
private:
	Game();

	static Game* mpGameInstance;

};