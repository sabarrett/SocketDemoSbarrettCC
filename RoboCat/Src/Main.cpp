
#include "RoboCatPCH.h"
#include "allegro_wrapper_functions-main/GraphicsLibrary.h"
#include "allegro_wrapper_functions-main/InputSystem.h"
#include "Bullet.h"
#include "Effect.h"
#include <chrono>
#include "Player.h"
#include <algorithm>
#include <mutex>

#if _WIN32

std::mutex mtx;

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

// Networking data
const string HOST_PORT_NUMBER = "8081";
const string CLIENT_PORT_NUMBER = "8084";

//void DoTcpServer(bool& shouldClose)
//{
//	
//
//	std::thread receiveThread([&quit, &connSocket, &connectedUsername]() {
//		while (!quit)
//		{
//			if (connSocket != nullptr)
//			{
//				char buffer[4096];
//				int32_t bytesReceived = connSocket->Receive(buffer, 4096);
//				if (bytesReceived == 0)
//				{
//					LOG("%s", " Connection gracefully closed. Press enter to exit!");
//					quit = true;
//					connSocket->Close();
//					connSocket = nullptr;
//					return;
//				}
//				if (bytesReceived < 0)
//				{
//					LOG("%s", " Connection forcefully closed. Press enter to exit!");
//					quit = true;
//					connSocket->Close();
//					connSocket = nullptr;
//					return;
//				}
//				std::string receivedMsg(buffer, bytesReceived);
//				if (receivedMsg == "---")
//				{
//					LOG(" User %s left chat room. Press enter to exit!", connectedUsername.c_str());
//					quit = true;
//					connSocket->Close();
//					connSocket = nullptr;
//					return;
//				}
//				else
//				{
//					LOG("%s", " Connection gracefully closed. Press enter to exit!");
//					LOG(" %s: %s", connectedUsername.c_str(), receivedMsg.c_str());
//				}
//			}
//		}
//		});
//
//	std::thread sendThread([&quit, &connSocket, &win]()
//		{
//			while (!quit)
//			{
//				if (connSocket != nullptr)
//				{
//					std::string input;
//					std::getline(std::cin, input);
//					if (!quit)
//						win.WriteFromStdin(" You: " + input);
//
//					if (input != "/exit")
//					{
//						connSocket->Send(input.c_str(), input.length());
//					}
//					else
//					{
//						std::string message("---");
//						connSocket->Send(message.c_str(), message.length());
//
//						LOG("%s", " Connection gracefully closed.  Press enter to exit!");
//						connSocket->Close();
//						connSocket = nullptr;
//						quit = true;
//					}
//				}
//			}
//		});
//
//	std::cout << " Type '/exit' and press Enter to exit!\n";
//
//	sendThread.join();
//	receiveThread.join();
//}
//
//
//
//void DoTcpClient()
//{
//	char buffer[4096];
//	int32_t bytesReceived = clientSocket->Receive(buffer, 4096);
//	if (bytesReceived == 0)
//	{
//		LOG("%s", " Connection gracefully closed. Press enter to exit!");
//		quit = true;
//		return;
//	}
//	if (bytesReceived < 0)
//	{
//		LOG("%s", " Connection forcefully closed. Press enter to exit!");
//		quit = true;
//		return;
//	}
//
//	std::string connectedUsername(buffer, bytesReceived);
//	LOG(" User %s joined chat room", connectedUsername.c_str());
//
//	std::thread receiveThread([&quit, &clientSocket, &connectedUsername]() {
//		while (!quit)
//		{
//			if (clientSocket != nullptr)
//			{
//				char buffer[4096];
//				int32_t bytesReceived = clientSocket->Receive(buffer, 4096);
//				if (bytesReceived == 0)
//				{
//					LOG("%s", " Connection gracefully closed. Press enter to exit!");
//					quit = true;
//					clientSocket->Close();
//					clientSocket = nullptr;
//					return;
//				}
//				if (bytesReceived < 0)
//				{
//					LOG("%s", " Connection forcefully closed. Press enter to exit!");
//					quit = true;
//					clientSocket->Close();
//					clientSocket = nullptr;
//					return;
//				}
//				std::string receivedMsg(buffer, bytesReceived);
//				if (receivedMsg == "---")
//				{
//					quit = true;
//					clientSocket->Close();
//					clientSocket = nullptr;
//					LOG(" User %s left chat room. Press any button to exit. Press enter to exit!", connectedUsername.c_str());
//					return;
//				}
//				else
//				{
//					LOG(" %s: %s", connectedUsername.c_str(), receivedMsg.c_str());
//				}
//			}
//		}
//		});
//
//	std::thread sendThread([&quit, &clientSocket, &win]()
//		{
//			while (!quit)
//			{
//				if (clientSocket != nullptr)
//				{
//					std::string input;
//					std::getline(std::cin, input);
//
//					if (!quit)
//						win.WriteFromStdin(" You: " + input);
//
//					if (input != "/exit")
//					{
//						clientSocket->Send(input.c_str(), input.length());
//					}
//					else
//					{
//						std::string message("---");
//						clientSocket->Send(message.c_str(), message.length());
//
//						LOG("%s", " Connection gracefully closed. Press enter to exit!");
//						quit = true;
//						clientSocket->Close();
//						clientSocket = nullptr;
//					}
//				}
//			}
//		});
//
//	std::cout << " Type '/exit' and press Enter to exit!\n";
//
//	sendThread.join();
//	receiveThread.join();
//}

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

			player1->SendPlayer(listenSocket, player1);

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
	}

	SocketUtil::CleanUp();

	std::cout << "Thank you for playing! Goodbye!" << std::endl;

	return 0;
}
