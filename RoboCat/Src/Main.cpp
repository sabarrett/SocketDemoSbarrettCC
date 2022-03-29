
#include "RoboCatPCH.h"
#include "allegro_wrapper_functions-main/GraphicsLibrary.h"
#include "allegro_wrapper_functions-main/InputSystem.h"
#include "Bullet.h"
#include "Effect.h"
#include <chrono>
#include "Player.h"
#include <algorithm>

#if _WIN32

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
const float RESOLUTION_X = 1920.0;
const float RESOLUTION_Y = 1080.0;

// Sprites sizes
const float PLAYER_SIZE = 80.0;
const float BULLET_SIZE = 30.0;
const float EFFECT_SIZE_X = 120.0;
const float EFFECT_SIZE_Y = 30.0;
const float OFFSET_HIT = 1.0;

// Objects speed
const float BULLET_SPEED = 0.5;
const float PLAYER_SPEED = 0.2;

// Effect duration
const float MAX_EFFECT_TIME = 700;
int main(int argc, const char** argv)
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#else
const char** __argv;
int __argc;
int main(int argc, const char** argv)
{
	__argc = argc;
	__argv = argv;
#endif

	SocketUtil::StaticInit();

	// ---------------------- General Game Data ----------------------
	bool isGameRunning = true;
	std::vector<Bullet*> bulletsVector;
	std::vector<Effect*> effectsVector;

	// ---------------------- Time related ----------------------
	clock_t start, end;
	float last_time = 0.0;
	float dt = 0.0;
	start = clock();
	
	// ---------------------- Intro Screen ----------------------
	string input = "";
	bool isHosting = false;

	std::cout << "Welcome to " << GAME_NAME << std::endl;
	std::cout << "To host the game, please, type 'h'" << std::endl;
	std::cout << "To join the game, please, type 'j'" << std::endl;

	do
	{
		std::cin >> input;
		
		if (input != "H" && input != "h" && input != "J" && input != "j")
		{
			std::cout << "Wrong input. Please, try again!" << std::endl;
			std::cout << "To host the game, please, type 'h'" << std::endl;
			std::cout << "To join the game, please, type 'j'" << std::endl;
		}
		else if (input == "H" || input == "h")
		{
			std::cout << "You will be the host. Waiting for players to join." << std::endl;
			isHosting = true;
		}
		else
		{
			std::cout << "Looking for the host to connect." << std::endl;
			isHosting = true;
		}

	} while (input != "H" && input != "h" && input != "J" && input != "j");

	// ---------------------- Graphics Library ----------------------
	GraphicsLibrary* pGL;
	pGL = new GraphicsLibrary(RESOLUTION_X, RESOLUTION_Y);
	if (!pGL->init())
	{
		std::cout << "Can't initialize Graphics Library" << std::endl;
		isGameRunning = false;
	}

	// ---------------------- Input System ----------------------
	InputSystem* pIS;
	pIS = new InputSystem();
	if (!pIS->init(pGL))
	{
		std::cout << "Can't initialize Input System" << std::endl;
		isGameRunning = false;
	}
	InputData inputData = InputData();

	// If Graphics and Input initialized correctly
	if (isGameRunning)
	{
		// Load Images
		pGL->loadImage(IMAGES_PATH + BACKGROUND_FILENAME, "background");
		pGL->loadImage(IMAGES_PATH + PLAYER_SPRITE_FILENAME, "player");
		pGL->loadImage(IMAGES_PATH + BULLET_SPRITE_FILENAME, "bullet");
		pGL->loadImage(IMAGES_PATH + BULLET_COVER_FILENAME, "bulletCover");
		pGL->loadImage(IMAGES_PATH + EFFECT_SPRITE_FILENAME, "effect");
		pGL->loadImage(IMAGES_PATH + EFFECT_COVER_FILENAME , "effectCover");

		// Draw Stuff
		pGL->drawImage("background", 0.0, 0.0);

		Player* player1 = new Player(0, RESOLUTION_X / 2 - PLAYER_SIZE / 2, RESOLUTION_Y * 8 / 10 + PLAYER_SIZE / 2, PLAYER_SPEED, "player");
		Player* player2 = new Player(1, RESOLUTION_X / 2 - PLAYER_SIZE / 2, RESOLUTION_Y * 1 / 10 - PLAYER_SIZE / 2, PLAYER_SPEED, "player");

		// ---------------------- Main Game Loop ----------------------
		while (isGameRunning)
		{
			// Calculating delta time since last update
			end = clock();
			last_time = ((float)end - start) / CLOCKS_PER_SEC;
			dt = (float)end / CLOCKS_PER_SEC - last_time;
			//std::cout << dt << std::endl;

			// Updates
			pIS->update();
			inputData = pIS->getInputData();
			for (auto& bullet : bulletsVector)
			{
				bullet->Update(dt);
				pGL->drawImage(bullet->mImageIdentifier, bullet->getPosX(), bullet->getPosY());
			}

			if (inputData.keyPressed_ESCAPE)
			{
				isGameRunning = false;
				//std::cout << "Escape Pressed" << std::endl;
			}
			if (inputData.keyPressed_R)
			{
				//std::cout << "R Pressed" << std::endl;
			}
			if (inputData.keyPressed_A)
			{
				//std::cout << "A Pressed" << std::endl;
				player1->Move(-dt * player1->mSpeed);
			}
			if (inputData.keyPressed_D)
			{
				//std::cout << "D Pressed" << std::endl;				
				player1->Move(dt * player1->mSpeed);

			}
			if (inputData.keyPressed_SPACE)
			{
				Bullet* newBullet = new Bullet(0, player1->getPosX() + PLAYER_SIZE / 2 - BULLET_SIZE / 2, player1->getPosY() - PLAYER_SIZE / 2, "bullet", BULLET_SPEED, true);
				pGL->drawImage(newBullet->mImageIdentifier, newBullet->getPosX(), newBullet->getPosY());
				bulletsVector.push_back(newBullet);
				std::cout << "Space Pressed" << std::endl;
			}

			for (auto& bullet : bulletsVector)
			{
				//check bullet y is <= player y + playersize
				//check bullet x is >= player x - bulletsize / 2 and <= player x + playersize
				if (bullet->getPosY() <= player2->getPosY() + PLAYER_SIZE + OFFSET_HIT &&
					bullet->getPosX() >= player2->getPosX() - BULLET_SIZE &&
					bullet->getPosX() <= player2->getPosX() + PLAYER_SIZE &&
					bullet->getPosY() >= player2->getPosY() - BULLET_SIZE / 2)
				{
					std::cout << "BULLET HIT";
					pGL->drawImage("bulletCover", bullet->getPosX(), bullet->getPosY());

					Effect* newEffect = new Effect(0, bullet->getPosX() + BULLET_SIZE / 2 - EFFECT_SIZE_X / 2, bullet->getPosY() - BULLET_SIZE / 2 + EFFECT_SIZE_Y / 2, "effect", true);
					pGL->drawImage(newEffect->mImageIdentifier, newEffect->getPosX(), newEffect->getPosY());
					effectsVector.push_back(newEffect);

					bullet = nullptr;
					bulletsVector.erase(std::remove(bulletsVector.begin(), bulletsVector.end(), bullet), bulletsVector.end());
				}
			}
			for (auto& effect : effectsVector)
			{
				effect->Update(dt);
				if (effect->mTimer >= MAX_EFFECT_TIME)
				{
					pGL->drawImage("effectCover", effect->getPosX(), effect->getPosY());
					effect = nullptr;
					effectsVector.erase(std::remove(effectsVector.begin(), effectsVector.end(), effect), effectsVector.end());
				}
			}

			pGL->drawImage(player1->mImageIdentifier, player1->getPosX(), player1->getPosY());
			pGL->drawImage(player2->mImageIdentifier, player2->getPosX(), player2->getPosY());
			pGL->render();
		}
	}

	SocketUtil::CleanUp();

	std::cout << "Thank you for playing! Goodbye!" << std::endl;

	return 0;
}
