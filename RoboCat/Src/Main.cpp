#include "RoboCatPCH.h"
#include "allegro_wrapper_functions-main/GraphicsLibrary.h"
#include "allegro_wrapper_functions-main/InputSystem.h"
#include "Bullet.h"
#include "Effect.h"
#include <chrono>
#include "Player.h"
#include "Networker.h"
#include "GameData.h"
#include <algorithm>
#include <mutex>

#if _WIN32

std::mutex mtx;

//Initialize pointer to zero so that it can be initialized in first call to getInstance
GameData* GameData::instance = 0;

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
	int gameObjectIDs = 2;

	Networker* pNetworker = new Networker();
	GameData* gameData = gameData->getInstance();
	
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
			isHosting = false;
		}

	} while (input != "H" && input != "h" && input != "J" && input != "j");

	// ------------------------------------------------------------------ Graphics Library ------------------------------------------------------------------
	GraphicsLibrary* pGL;
	pGL = new GraphicsLibrary(RESOLUTION_X, RESOLUTION_Y);
	if (!pGL->init())
	{
		std::cout << "Can't initialize Graphics Library" << std::endl;
		isGameRunning = false;
	}

	// ------------------------------------------------------------------ Input System ------------------------------------------------------------------
	InputSystem* pIS;
	pIS = new InputSystem();
	if (!pIS->init(pGL))
	{
		std::cout << "Can't initialize Input System" << std::endl;
		isGameRunning = false;
	}
	InputData inputData = InputData();

	if (isHosting) // ------------------------------------------------------------------ HOST ------------------------------------------------------------------
	{
		// Create listening socket
		TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
		if (listenSocket == nullptr)
		{
			SocketUtil::ReportError(" Unable to create a listening socket");
			ExitProcess(1);
		}
		LOG("%s", " Listening socket created");

		// Create listening address
		SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:" + HOST_PORT_NUMBER);
		if (listenAddress == nullptr)
		{
			SocketUtil::ReportError(" Unable to create a listen address");
			ExitProcess(1);
		}

		// Bind address to socket
		if (listenSocket->Bind(*listenAddress) != NO_ERROR)
		{
			SocketUtil::ReportError(" Unable to bind listening socket");
			ExitProcess(1);
		}
		LOG("%s", " Bound listening socket");

		// Start listening for incoming connections
		if (listenSocket->Listen() != NO_ERROR)
		{
			SocketUtil::ReportError(" Unable to start listening on a listening socket");
			ExitProcess(1);
		}
		LOG("%s", " Listening on socket");

		// Wait for incoming connection
		SocketAddress incomingAddress;
		TCPSocketPtr connSocket = listenSocket->Accept(incomingAddress);
		LOG("%s", " Waiting for connections...");

		while (connSocket == nullptr)
		{
			connSocket = listenSocket->Accept(incomingAddress);

			// SocketUtil::ReportError("No incoming connection");
			// ExitProcess(1);
		}
		LOG(" Accepted connection from %s", incomingAddress.ToString().c_str());

		pNetworker->SetSocket(connSocket);

		// If Graphics and Input initialized correctly
		if (isGameRunning)
		{
			// Load Images
			pGL->loadImage(IMAGES_PATH + BACKGROUND_FILENAME, "background");
			pGL->loadImage(IMAGES_PATH + PLAYER_SPRITE_FILENAME, "player");
			pGL->loadImage(IMAGES_PATH + BULLET_SPRITE_FILENAME, "bullet");
			pGL->loadImage(IMAGES_PATH + BULLET_COVER_FILENAME, "bulletCover");
			pGL->loadImage(IMAGES_PATH + EFFECT_SPRITE_FILENAME, "effect");
			pGL->loadImage(IMAGES_PATH + EFFECT_COVER_FILENAME, "effectCover");

			// Draw Stuff
			pGL->drawImage("background", 0.0, 0.0);

			gameData->player1 = new Player(0, RESOLUTION_X / 2 - PLAYER_SIZE / 2, RESOLUTION_Y * 8 / 10 + PLAYER_SIZE / 2, PLAYER_SPEED, "player");
			gameData->player2 = new Player(1, RESOLUTION_X / 2 - PLAYER_SIZE / 2, RESOLUTION_Y * 1 / 10 - PLAYER_SIZE / 2, PLAYER_SPEED, "player");
			pGL->drawImage(gameData->player2->mImageIdentifier, gameData->player2->mPosX, gameData->player2->mPosY);

			std::thread receiveThread([&isGameRunning, &connSocket, &gameObjectIDs, &pGL, &gameData]()
				{
					while (isGameRunning)
					{
						if (connSocket != nullptr)
						{							
							switch (pNetworker->ReceivePacket(gameObjectIDs, isGameRunning))
							{
								case GameObjectType::PLAYER:
								{
									if (player2->GetIsFiring())
									{
										Bullet* newBullet = new Bullet(gameObjectIDs, player2->mPosX + PLAYER_SIZE / 2 - BULLET_SIZE / 2, player2->mPosY + PLAYER_SIZE * 3 / 2, "bullet", BULLET_SPEED, false);
										gameObjectIDs++;
										bulletsVector.push_back(newBullet);
									}
									break;
								}
							}
						}
					}
				});

			// ---------------------- Time related ----------------------
			clock_t start, end;
			float last_time = 0.0;
			float dt = 0.0;
			start = clock();

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
						if (bullet != nullptr)
						{
  							pGL->drawImage("bullet", bullet->mPosX, bullet->mPosY);
							bullet->Update(dt);
							bullet->SendBullet(connSocket);
						}
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
					player1->Move(-dt * player1->GetSpeed());
					player1->SendPlayer(connSocket);
					//std::cout << "dt = " << dt << std::endl;
					//std::cout << "speed = " << player1->GetSpeed() << std::endl;
				}
				if (inputData.keyPressed_D)
				{
					//std::cout << "D Pressed" << std::endl;				
					player1->Move(dt * player1->GetSpeed());
					player1->SendPlayer(connSocket);

				}
				if (inputData.keyPressed_SPACE)
				{
					Bullet* newBullet = new Bullet(gameObjectIDs, player1->mPosX + PLAYER_SIZE / 2 - BULLET_SIZE / 2, player1->mPosY - PLAYER_SIZE / 2, "bullet", BULLET_SPEED, true);
					gameObjectIDs++;
					pGL->drawImage(newBullet->mImageIdentifier, newBullet->mPosX, newBullet->mPosX);
					bulletsVector.push_back(newBullet);
					//std::cout << "Space Pressed" << std::endl;
				}

				
					for (auto& bullet : bulletsVector)
					{
						if (bullet != nullptr)
						{
							//check bullet y is <= player y + playersize
							//check bullet x is >= player x - bulletsize / 2 and <= player x + playersize
							if (bullet->mPosY <= player2->mPosY + PLAYER_SIZE + OFFSET_HIT &&
								bullet->mPosX >= player2->mPosX - BULLET_SIZE &&
								bullet->mPosX <= player2->mPosX + PLAYER_SIZE &&
								bullet->mPosY >= player2->mPosY - BULLET_SIZE / 2)
							{
								//std::cout << "BULLET HIT";
								pGL->drawImage("bulletCover", bullet->mPosX, bullet->mPosY);

								Effect* newEffect = new Effect(gameObjectIDs, bullet->mPosX + BULLET_SIZE / 2 - EFFECT_SIZE_X / 2, bullet->mPosY - BULLET_SIZE / 2 + EFFECT_SIZE_Y / 2, "effect", true);
								newEffect->SendEffect(connSocket);
								gameObjectIDs++;
								pGL->drawImage(newEffect->mImageIdentifier, newEffect->mPosX, newEffect->mPosY);
								effectsVector.push_back(newEffect);

								bullet = nullptr;
								bulletsVector.erase(std::remove(bulletsVector.begin(), bulletsVector.end(), bullet), bulletsVector.end());
							}
							else if (bullet->mPosY <= player1->mPosY + PLAYER_SIZE &&
								bullet->mPosX >= player1->mPosX - BULLET_SIZE &&
								bullet->mPosX <= player1->mPosX + PLAYER_SIZE &&
								bullet->mPosY >= player1->mPosY - BULLET_SIZE / 2 - OFFSET_HIT)
							{
								//std::cout << "BULLET HIT";
								pGL->drawImage("bulletCover", bullet->mPosX, bullet->mPosY);

								Effect* newEffect = new Effect(gameObjectIDs, bullet->mPosX + BULLET_SIZE / 2 - EFFECT_SIZE_X / 2, bullet->mPosY - BULLET_SIZE / 2 + EFFECT_SIZE_Y / 2, "effect", true);
								newEffect->SendEffect(connSocket);
								gameObjectIDs++;
								pGL->drawImage(newEffect->mImageIdentifier, newEffect->mPosX, newEffect->mPosY);
								effectsVector.push_back(newEffect);

								bullet->gotDestroyed = true;
								bullet->SendBullet(connSocket);

								bullet = nullptr;
								bulletsVector.erase(std::remove(bulletsVector.begin(), bulletsVector.end(), bullet), bulletsVector.end());
							}
							else if (bullet->mPosY <= 0 || bullet->mPosY >= RESOLUTION_Y)
							{
								pGL->drawImage("bulletCover", bullet->mPosX, bullet->mPosY);
								bullet->gotDestroyed = true;
								bullet->SendBullet(connSocket);

								bullet = nullptr;
								bulletsVector.erase(std::remove(bulletsVector.begin(), bulletsVector.end(), bullet), bulletsVector.end());
							}
						}
					}
				

		
					for (auto& effect : effectsVector)
					{
						if (effect != nullptr)
						{
							effect->Update(dt);
							if (effect->mTimer >= MAX_EFFECT_TIME)
							{
								pGL->drawImage("effectCover", effect->mPosX, effect->mPosY);
								effect = nullptr;
								effectsVector.erase(std::remove(effectsVector.begin(), effectsVector.end(), effect), effectsVector.end());
							}
						}
					}
				

				pGL->drawImage(player1->mImageIdentifier, player1->mPosX, player1->mPosY);
				pGL->drawImage(player2->mImageIdentifier, player2->mPosX, player2->mPosY);
				pGL->render();
			}
		}
		connSocket->Close();
		listenSocket->Close();
		connSocket = nullptr;
		listenSocket = nullptr;
	}
	else // ------------------------------------------------------------------ CLIENT ------------------------------------------------------------------
	{
		// Create socket for client
		TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
		if (clientSocket == nullptr)
		{
			SocketUtil::ReportError(" Unable to create a client socket");
			ExitProcess(1);
		}
		LOG("%s", " Client socket created");

		// Create address for client
		std::string address = StringUtils::Sprintf("127.0.0.1:%s", CLIENT_PORT_NUMBER.c_str());
		SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
		if (clientAddress == nullptr)
		{
			SocketUtil::ReportError(" Unable to create client address");
			ExitProcess(1);
		}

		// Bind address to socket
		if (clientSocket->Bind(*clientAddress) != NO_ERROR)
		{
			SocketUtil::ReportError(" Unable to bind client socket");
			ExitProcess(1);
		}
		LOG("%s", " Bound client socket");

		// Create address for server
		SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:" + HOST_PORT_NUMBER);
		if (servAddress == nullptr)
		{
			SocketUtil::ReportError(" Unable to create server address");
			ExitProcess(1);
		}

		// Connect socket to remote host
		if (clientSocket->Connect(*servAddress) != NO_ERROR)
		{
			SocketUtil::ReportError(" No connection to server");
			ExitProcess(1);
		}
		LOG("%s", " Connected to server!");

		pNetworker->SetSocket(clientSocket);

		// If Graphics and Input initialized correctly
		if (isGameRunning)
		{
			// Load Images
			pGL->loadImage(IMAGES_PATH + BACKGROUND_FILENAME, "background");
			pGL->loadImage(IMAGES_PATH + PLAYER_SPRITE_FILENAME, "player");
			pGL->loadImage(IMAGES_PATH + BULLET_SPRITE_FILENAME, "bullet");
			pGL->loadImage(IMAGES_PATH + BULLET_COVER_FILENAME, "bulletCover");
			pGL->loadImage(IMAGES_PATH + EFFECT_SPRITE_FILENAME, "effect");
			pGL->loadImage(IMAGES_PATH + EFFECT_COVER_FILENAME, "effectCover");

			// Draw Stuff
			pGL->drawImage("background", 0.0, 0.0);

			Player* player1 = new Player(0, RESOLUTION_X / 2 - PLAYER_SIZE / 2, RESOLUTION_Y * 8 / 10 + PLAYER_SIZE / 2, PLAYER_SPEED, "player");
			Player* player2 = new Player(1, RESOLUTION_X / 2 - PLAYER_SIZE / 2, RESOLUTION_Y * 1 / 10 - PLAYER_SIZE / 2, PLAYER_SPEED, "player");
			pGL->drawImage(player2->mImageIdentifier, player2->mPosX, player2->mPosY);

			std::thread receiveThread([&isGameRunning, &clientSocket, &bulletsVector, &gameObjectIDs, &player2, &effectsVector, &pGL]()
				{
					while (isGameRunning)
					{
						if (clientSocket != nullptr)
						{
							switch (ReceivePacket(clientSocket, gameObjectIDs, player2, &bulletsVector, &effectsVector, isGameRunning))
							{
							case GameObjectType::BULLET:
							{
								//std::cout << "Received a bullet" << std::endl;
								//for (auto& bullet : bulletsVector)
								//{
								//	if (bullet != nullptr)
								//	{
								//		if (bullet->gotDestroyed)
								//		{
								//			//std::cout << "Bullet got destroyed" << std::endl;
								//			//pGL->drawImage("bulletCover", bullet->getPosX(), bullet->getPosY());
								//			bulletsVector.erase(std::remove(bulletsVector.begin(), bulletsVector.end(), bullet), bulletsVector.end());
								//			bullet = nullptr;
								//		}
								//		else
								//		{
								//			bullet->mImageIdentifier = "bullet";
								//		}
								//	}
								//}
							}
							}
						}
					}
				});

			// ---------------------- Time related ----------------------
			clock_t start, end;
			float last_time = 0.0;
			float dt = 0.0;
			start = clock();

			// ---------------------- Main Game Loop ----------------------
			while (isGameRunning)
			{
				// Calculating delta time since last update
				end = clock();
				last_time = ((float)end - start) / CLOCKS_PER_SEC;
				dt = (float)end / CLOCKS_PER_SEC - last_time;

				// Updates
				pIS->update();
				inputData = pIS->getInputData();


				for (auto& bullet : bulletsVector)
				{
					if (bullet != nullptr)
					{
						pGL->drawImage("bullet", bullet->mPosX, bullet->mPosY);
						//std::cout << bullet->mPosX << "   " << bullet->mPosY << std::endl;
						if (bullet->gotDestroyed)
						{
							//std::cout << "Bullet got destroyed" << std::endl;
							//pGL->drawImage("bulletCover", bullet->getPosX(), bullet->getPosY());
							bulletsVector.erase(std::remove(bulletsVector.begin(), bulletsVector.end(), bullet), bulletsVector.end());
							bullet = nullptr;
						}
					}
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
					player1->Move(-dt * player1->GetSpeed());
					player1->SendPlayer(clientSocket);
				}
				if (inputData.keyPressed_D)
				{
					//std::cout << "D Pressed" << std::endl;				
					player1->Move(dt * player1->GetSpeed());
					player1->SendPlayer(clientSocket);
				}
				if (inputData.keyPressed_SPACE)
				{
					//player1->SetIsFiring(true);
					//player1->SendPlayer(clientSocket);
					//std::cout << "Space Pressed" << std::endl;
				}


				for (auto& effect : effectsVector)
				{
					if (effect != nullptr)
					{
						pGL->drawImage("effect", effect->mPosX, effect->mPosY);
						effect->Update(dt);
						if (effect->mTimer >= MAX_EFFECT_TIME)
						{
							pGL->drawImage("effectCover", effect->mPosX, effect->mPosY);
							effect = nullptr;
							effectsVector.erase(std::remove(effectsVector.begin(), effectsVector.end(), effect), effectsVector.end());
						}
					}
				}


				pGL->drawImage(player1->mImageIdentifier, player1->mPosX, player1->mPosY);
				pGL->drawImage(player2->mImageIdentifier, player2->mPosX, player2->mPosY);
				pGL->render();
			}
			clientSocket->Close();
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DONT FORGET TO CLEANUP !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	SocketUtil::CleanUp();

	std::cout << "Thank you for playing! Goodbye!" << std::endl;

	return 0;
}
