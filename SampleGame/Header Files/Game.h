#pragma once
#include "Trackable.h"
#include "InputSystem.h"
#include <string>

class GraphicsSystem;
class InputSystem;
class Player;
class Timer;
class UnitManager;
class AnimationManager;
class GraphicsBufferManager;
class GameListener;
class EventSystem;
class Vector2D;
class TCPNetworkManager;
class Unit;

enum Packet_Header;

const std::string ASSET_PATH = GAMEASSETS;
const std::string SMURF_FILENAME = "smurf_sprites.png";
const std::string PROJECTILE_FILENAME = "Sphere_Glow.png";
const std::string BACKGROUND_FILEPATH = "room\\room0000.png";

const float PROJECTILE_SPEED = 100.0f;

class Game : public Trackable
{
public:
	friend class GameListener;

	static Game* getInstance();
	static void cleanupInstance();

	void init(int screenWidth, int screenHeight, int fps = 60, bool isServer = false, bool debugMode = false);
	void cleanup();

	void startGame();

private:
	Game();
	~Game();

	void getInput();
	void update();
	void render();

	void debug(); //Just a bunch of stuff to do in Debug Mode

	void DPlayerMove(Vector2D loc); //Functions that begin with the prefix D are debug functions
	void DKeyPress(KeyCode);
	void DMousePress(int);
	void DKeyRelease(KeyCode);
	void DMouseRelease(int);

	int fireProj();
	void fireOppProj(int id, Vector2D loc);

	void quitGame();

	static void handleNetworkPacket(Packet_Header, char* data, int length);

	static Game* mspInstance;

	GraphicsSystem* mpGraphicsSystem;

	UnitManager* mpUnitManager;
	AnimationManager* mpAnimationManager;
	GraphicsBufferManager* mpGraphicsBufferManager;

	GameListener* mpGameListener;

	Player* mpPlayerUnit;
	Unit* mpOpponent;

	Timer* mpGameTimer;

	double deltaTime;
	bool mDebugMode, mIsPlaying, mIsServer;

	float mTimePerFrame;

	unsigned int playerPacketTime;
	
};
