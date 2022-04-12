#include "Bullet.h"
#include "Effect.h"
#include "Player.h"
#pragma once

// Name of the game
const string GAME_NAME = "temporary name";

// Filenames and paths
const string IMAGES_PATH = "Images/";
const string BACKGROUND_FILENAME = "Background.png";
const string PLAYER_SPRITE_FILENAME = "PlayerSprite.png";
const string BULLET_SPRITE_FILENAME = "BulletSprite.png";
const string BULLET_COVER_FILENAME = "BulletCover.png";
const string EFFECT_SPRITE_FILENAME = "Effect.png";
const string EFFECT_COVER_FILENAME = "EffectCover.png";

// Screen Resolution
const float RESOLUTION_X = 800.0;
const float RESOLUTION_Y = 600.0;

// Sprites sizes
const float PLAYER_SIZE = 80.0;
const float BULLET_SIZE = 30.0;
const float EFFECT_SIZE_X = 120.0;
const float EFFECT_SIZE_Y = 30.0;
const float OFFSET_HIT = 1.0;

// Objects speed
const float BULLET_SPEED = 0.01;
const float PLAYER_SPEED = 0.02;

// Networking data
const string HOST_PORT_NUMBER = "8082";
const string CLIENT_PORT_NUMBER = "8085";

// Effect duration
const float MAX_EFFECT_TIME = 7000;

class GameData
{
protected:
	GameData() {};
	static GameData* instance_;
	TCPSocketPtr inSocket;

public:
	GameData(GameData& other) = delete;
	void operator=(const GameData&) = delete;

	static GameData* getInstance();

	void resendAllEffects();

	void SetSocket(TCPSocketPtr newSocket);

	Player* player1 = nullptr;
	Player* player2 = nullptr;

	std::vector<Bullet*> bulletsVector;
	std::vector<Effect*> effectsVector;
};