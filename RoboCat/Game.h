#pragma once
#include "Trackable.h"
#include <Timer.h>
#include <assert.h>
#include <PerformanceTracker.h>
#include "Sprite.h"
#include "EventListener.h"

using namespace std;

class Animation;
class Event;
class EventListener;
class EventSystem;
class GraphicsBuffer;
class GraphicsBufferManager;
class InputSystem;
class InputTranslator;
class Sprite;
class System;
class Unit;
class UnitManager;


const string ASSET_FILEPATH = "..\\..\\common\\assets\\";
const string SMURF_FILEPATH = "smurf_sprites.png";
const string DEAN_FILEPATH = "dean_sprites.png";
const string WOODS_FILEPATH = "Woods.png";
const float ANIM_INCREMENT_AMOUNT = 5;
const float WOODS_SCALE = 0.5f;

// The class which holds all systems and has the game loop
class Game : public EventListener
{
public:
	static void initInstance();
	static Game* getInstance(); // reference to the main game object. To use: Game* ptr = Game::getInstance(); where ptr is now the game object
	static void deleteInstance();

	void init();
	void cleanup();
	void doLoop();

	void handleEvent(const Event& event);

	void update();
	void render();

	System* getSystem() { return mpSystem; }
private:
	Game();
	~Game();

	System* mpSystem;

	InputTranslator* mpInputTranslator;
	EventListener* mpEventListener;

	GraphicsBuffer* mpWoodsBuffer = nullptr;
	GraphicsBuffer* mpSmurfBuffer = nullptr;
	GraphicsBuffer* mpDeanBuffer = nullptr;
	GraphicsBuffer* mpCurrentBuffer = nullptr;
	GraphicsBufferManager* mpGraphicsBufferManager;

	Animation* mpTempAnim;
	Unit* mpTempUnit;

	Sprite* mpWoodsSprite;

	Unit* mpUnit;
	UnitManager* mpUnitManager;

	Sprite mSmurfSpriteArr[16];
	Sprite mDeanSpriteArr[16];

	float mFrameRate = 60;
	float mAnimSpeed;

	bool mShouldExit;
	bool mIsSmurf;
	bool mIsPaused;

	Timer* mpTimer;
	PerformanceTracker* mpPerformanceTracker;

	static Game* gpGame;
};