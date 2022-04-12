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
//GameData* GameData::instance = 0;

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
	SocketUtil::CleanUp();
	SocketUtil::StaticInit();
	// ---------------------- General Game Data ----------------------
	bool isGameRunning = true;
	int gameObjectIDs = 2;

	Networker* pNetworker = new Networker();
	GameData* gameData = GameData::getInstance();
	
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
	DeliveryNotificationManager* deliveryNotificationManagerPtr = new DeliveryNotificationManager(true, true);
	pNetworker->pDeliveryNotificationManager = deliveryNotificationManagerPtr;

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

		//connSocket->SetNonBlockingMode(true);
		gameData->SetSocket(connSocket);
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
			gameData->player1->pDeliveryNotificationManager = deliveryNotificationManagerPtr;
			gameData->player2->pDeliveryNotificationManager = deliveryNotificationManagerPtr;
			pGL->drawImage(gameData->player1->mImageIdentifier, gameData->player1->getX(), gameData->player1->getY());
			pGL->drawImage(gameData->player2->mImageIdentifier, gameData->player2->getX(), gameData->player2->getY());

			std::thread receiveThread([&isGameRunning, pNetworker]()
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1000));

					while (isGameRunning)
					{
						pNetworker->ReceivePacketIntoQueue();
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

				//pGL->drawImage("background", 0.0, 0.0);
				pGL->drawImage(gameData->player1->mImageIdentifier, gameData->player1->getX(), gameData->player1->getY());
				pGL->drawImage(gameData->player2->mImageIdentifier, gameData->player2->getX(), gameData->player2->getY());

				//std::cout << gameData->bulletsVector.size() << std::endl;

				for (auto& bullet : gameData->bulletsVector)
				{
					if (bullet != nullptr)
					{
						pGL->drawImage("bullet", bullet->getX(), bullet->getY());
						bullet->Update(dt);
						//std::cout << "Bullet spawn 2" << std::endl;
						bullet->SendBullet(connSocket);
					}
				}

				if (inputData.keyPressed_ESCAPE)
				{
					std::cout << "Escape Pressed" << std::endl;

					OutputMemoryBitStream stream;
					stream.Write(GameObjectType::END_OF_GAME);

					connSocket->Send(stream.GetBufferPtr(), stream.GetBitLength());
					isGameRunning = false;
				}
				if (inputData.keyPressed_A)
				{
					//std::cout << "A Pressed" << std::endl;
					gameData->player1->Move(-dt * gameData->player1->GetSpeed());
					gameData->player1->SendPlayer(connSocket);
				}
				if (inputData.keyPressed_D)
				{
					//std::cout << "D Pressed" << std::endl;				
					gameData->player1->Move(dt * gameData->player1->GetSpeed());
					gameData->player1->SendPlayer(connSocket);

				}
				if (inputData.keyPressed_SPACE)
				{
					//std::cout << "Bullet spawn 3" << std::endl;
					Bullet* newBullet = new Bullet(gameObjectIDs, gameData->player1->getX() + PLAYER_SIZE / 2 - BULLET_SIZE / 2, gameData->player1->getY() - PLAYER_SIZE / 2, "bullet", BULLET_SPEED, true);
					newBullet->pDeliveryNotificationManager = deliveryNotificationManagerPtr;
					gameObjectIDs++;
					pGL->drawImage(newBullet->mImageIdentifier, newBullet->getX(), newBullet->getX());
					gameData->bulletsVector.push_back(newBullet);
					//std::cout << "Space Pressed" << std::endl;
				}

				for (auto& bullet : gameData->bulletsVector)
				{
					if (bullet != nullptr)
					{
						// If bullet hits client's player
						if (bullet->getY() <= gameData->player2->getY() + PLAYER_SIZE + OFFSET_HIT &&
							bullet->getX() >= gameData->player2->getX() - BULLET_SIZE &&
							bullet->getX() <= gameData->player2->getX() + PLAYER_SIZE &&
							bullet->getY() >= gameData->player2->getY() - BULLET_SIZE / 2)
						{
							//std::cout << "BULLET HIT";
							pGL->drawImage("bulletCover", bullet->getX(), bullet->getY());

							Effect* newEffect = new Effect(gameObjectIDs, bullet->getX() + BULLET_SIZE / 2 - EFFECT_SIZE_X / 2, bullet->getY() - BULLET_SIZE / 2 + EFFECT_SIZE_Y / 2, "effect", true);
							newEffect->pDeliveryNotificationManager = deliveryNotificationManagerPtr;
							newEffect->SendEffect(connSocket);
							gameObjectIDs++;
							pGL->drawImage(newEffect->mImageIdentifier, newEffect->getX(), newEffect->getY());
							gameData->effectsVector.push_back(newEffect);

							delete bullet;
							bullet = nullptr;
							gameData->bulletsVector.erase(std::remove(gameData->bulletsVector.begin(), gameData->bulletsVector.end(), nullptr), gameData->bulletsVector.end());
						}
						// If bullet hits host's player
						else if (bullet->getY() <= gameData->player1->getY() + PLAYER_SIZE &&
							bullet->getX() >= gameData->player1->getX() - BULLET_SIZE &&
							bullet->getX() <= gameData->player1->getX() + PLAYER_SIZE &&
							bullet->getY() >= gameData->player1->getY() - BULLET_SIZE / 2 - OFFSET_HIT)
						{
							//std::cout << "BULLET HIT";
							pGL->drawImage("bulletCover", bullet->getX(), bullet->getY());

							Effect* newEffect = new Effect(gameObjectIDs, bullet->getX() + BULLET_SIZE / 2 - EFFECT_SIZE_X / 2, bullet->getY() - BULLET_SIZE / 2 + EFFECT_SIZE_Y / 2, "effect", true);
							newEffect->pDeliveryNotificationManager = deliveryNotificationManagerPtr;
							newEffect->SendEffect(connSocket);
							gameObjectIDs++;
							pGL->drawImage(newEffect->mImageIdentifier, newEffect->getX(), newEffect->getY());
							gameData->effectsVector.push_back(newEffect);

							bullet->gotDestroyed = true;
							//std::cout << "Bullet spawn 4" << std::endl;
							bullet->SendBullet(connSocket);

							delete bullet;
							bullet = nullptr;
							gameData->bulletsVector.erase(std::remove(gameData->bulletsVector.begin(), gameData->bulletsVector.end(), nullptr), gameData->bulletsVector.end());
						}
						else if (bullet->getY() <= 0 || bullet->getY() >= RESOLUTION_Y)
						{
							//std::cout << "Bullet spawn 5" << std::endl;
							pGL->drawImage("bulletCover", bullet->getX(), bullet->getY());
							bullet->gotDestroyed = true;
							bullet->SendBullet(connSocket);

							delete bullet;
							bullet = nullptr;
							gameData->bulletsVector.erase(std::remove(gameData->bulletsVector.begin(), gameData->bulletsVector.end(), nullptr), gameData->bulletsVector.end());
						}
					}
				}

				for (auto& effect : gameData->effectsVector)
				{
					if (effect != nullptr)
					{
						effect->Update(dt);
						if (effect->mTimer >= MAX_EFFECT_TIME)
						{
							pGL->drawImage("effectCover", effect->getX(), effect->getY());

							delete effect;
							effect = nullptr;
							gameData->effectsVector.erase(std::remove(gameData->effectsVector.begin(), gameData->effectsVector.end(), nullptr), gameData->effectsVector.end());
						}
					}
				}

				pNetworker->ProcessQueuedPackets(gameObjectIDs, isGameRunning);

				pGL->drawImage(gameData->player1->mImageIdentifier, gameData->player1->getX(), gameData->player1->getY());
				pGL->drawImage(gameData->player2->mImageIdentifier, gameData->player2->getX(), gameData->player2->getY());
				pGL->render();
			}

			receiveThread.join();
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

		gameData->SetSocket(clientSocket);
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

			gameData->player1 = new Player(0, RESOLUTION_X / 2 - PLAYER_SIZE / 2, RESOLUTION_Y * 8 / 10 + PLAYER_SIZE / 2, PLAYER_SPEED, "player");
			gameData->player2 = new Player(1, RESOLUTION_X / 2 - PLAYER_SIZE / 2, RESOLUTION_Y * 1 / 10 - PLAYER_SIZE / 2, PLAYER_SPEED, "player");
			gameData->player1->pDeliveryNotificationManager = deliveryNotificationManagerPtr;
			gameData->player2->pDeliveryNotificationManager = deliveryNotificationManagerPtr;

			pGL->drawImage(gameData->player2->mImageIdentifier, gameData->player2->getX(), gameData->player2->getY());

			std::thread receiveThread([&isGameRunning, pNetworker]()
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1000));

					while (isGameRunning)
					{
						pNetworker->ReceivePacketIntoQueue();
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

				//pGL->drawImage("background", 0.0, 0.0);
				pGL->drawImage(gameData->player1->mImageIdentifier, gameData->player1->getX(), gameData->player1->getY());
				pGL->drawImage(gameData->player2->mImageIdentifier, gameData->player2->getX(), gameData->player2->getY());


				//gameData->bulletsVector.erase(std::remove(gameData->bulletsVector.begin(), gameData->bulletsVector.end(), nullptr), gameData->bulletsVector.end());
				for (auto& bullet : gameData->bulletsVector)
				{
					//gameData->bulletsVector.erase(std::remove(gameData->bulletsVector.begin(), gameData->bulletsVector.end(), nullptr), gameData->bulletsVector.end());
					if (bullet != nullptr)
					{
						pGL->drawImage("bullet", bullet->getX(), bullet->getY());
						//std::cout << bullet->mPosX << "   " << bullet->mPosY << std::endl;
					}
				}


				if (inputData.keyPressed_ESCAPE)
				{
					std::cout << "Escape Pressed" << std::endl;
					OutputMemoryBitStream stream;
					stream.Write(GameObjectType::END_OF_GAME);
					clientSocket->Send(stream.GetBufferPtr(), stream.GetBitLength());
					isGameRunning = false;
				}
				if (inputData.keyPressed_R)
				{
					//std::cout << "R Pressed" << std::endl;
				}
				if (inputData.keyPressed_A)
				{
					//std::cout << "A Pressed" << std::endl;
					gameData->player1->Move(-dt * gameData->player1->GetSpeed());
					gameData->player1->SendPlayer(clientSocket);
				}
				if (inputData.keyPressed_D)
				{
					//std::cout << "D Pressed" << std::endl;				
					gameData->player1->Move(dt * gameData->player1->GetSpeed());
					gameData->player1->SendPlayer(clientSocket);
				}
				if (inputData.keyPressed_SPACE)
				{
					//player1->SetIsFiring(true);
					//player1->SendPlayer(clientSocket);
					//std::cout << "Space Pressed" << std::endl;
				}


				for (auto& effect : gameData->effectsVector)
				{
					if (effect != nullptr)
					{
						pGL->drawImage("effect", effect->getX(), effect->getY());
						effect->Update(dt);
						if (effect->mTimer >= MAX_EFFECT_TIME)
						{
							pGL->drawImage("effectCover", effect->getX(), effect->getY());
							delete effect;
							effect = nullptr;
							gameData->effectsVector.erase(std::remove(gameData->effectsVector.begin(), gameData->effectsVector.end(), nullptr), gameData->effectsVector.end());
						}
					}
				}
				
				pNetworker->ProcessQueuedPackets(gameObjectIDs, isGameRunning);

				pGL->drawImage(gameData->player1->mImageIdentifier, gameData->player1->getX(), gameData->player1->getY());
				pGL->drawImage(gameData->player2->mImageIdentifier, gameData->player2->getX(), gameData->player2->getY());
				pGL->render();
			}

			//receiveThread.join();
			clientSocket->Close();
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! DONT FORGET TO CLEANUP !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	SocketUtil::CleanUp();

	std::cout << "Thank you for playing! Goodbye!" << std::endl;

	return 0;
}
