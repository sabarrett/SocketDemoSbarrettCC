//
//#include "RoboCatPCH.h"
//
//SDL_Renderer* gRenderer;
//
////#if _WIN32
////int main(int argc, const char** argv)
////{
////	UNREFERENCED_PARAMETER(argc);
////	UNREFERENCED_PARAMETER(argv);
////#else
////const char** __argv;
////int __argc;
////int main(int argc, const char** argv)
////{
////	__argc = argc;
////	__argv = argv;
////#endif
////
////	SocketUtil::StaticInit();
////
////	SocketUtil::CleanUp();
////
////	return 0;
////}

///*
// A lot of the SDL code I was unfamiliar with, so I used https://lazyfoo.net/tutorials/SDL/ as a guide.
// I followed the tutorials to learn SDL, so my code is similar.
//*/
//

#include "RoboCatPCH.h"

bool init();
bool loadMedia();
void close();

const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 540;

SDL_Window* gWindow = NULL;

SDL_Renderer* gRenderer;

Player player1;
Player player2;
Crosshair crosshair1;
Crosshair crosshair2;
std::vector<Bullet> bullets1;
std::vector<Bullet> bullets2;
// iterators get invalidated when .push_back() is called, which breaks render() method because it takes a long time
// need to iterate with index instead
int bullets2Size = 0;

bool init()
{
	srand(time(NULL));
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Failure initializing SDL.  Error: %s\n", SDL_GetError());
	}
	else
	{
		gWindow = SDL_CreateWindow( "Assignment 2/3 Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Failure initializing Window.  Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			// create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Failure creaing renderer.  Error: %s\n", SDL_GetError());
			}
			else
			{
				SDL_SetRenderDrawColor(gRenderer, 0x0, 0x0, 0x0, 0xFF);

				// IMG_Init returns a number, IMG_INIT_PNG == 2.  bitwise & to compare
				if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
				{
					printf("Failure initializing SDL_image.  Error %s\n", SDL_GetError());
					success = false;
				}

				SDL_ShowCursor(false);
			}
		}
	}
	return success;
}

bool loadMedia()
{
	bool success = true;

	if (!Player::playerTexture.loadFromFile("images/player.png"))
	{
		printf("Failed to load player texture");
		success = false;
	}

	if (!Crosshair::crosshairTexture.loadFromFile("images/crosshair.png"))
	{
		printf("Failed to load crosshair texture");
		success = false;
	}

	if (!Bullet::bulletTexture.loadFromFile("images/bullet.png"))
	{
		printf("Failed to load bullet texture");
		success = false;
	}

	return success;
}

void close()
{
	Player::playerTexture.free();
	Crosshair::crosshairTexture.free();

	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;

	IMG_Quit();
	SDL_Quit();
}

void startServer(SocketAddressPtr serverAddress)
{
	UDPSocketPtr serverSocket = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);

	SocketAddressPtr client1Address = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8081");
	SocketAddressPtr client2Address = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8082");

	serverSocket->Bind(*serverAddress);

	std::cout << "Waiting for packet.\n";

	std::thread serverThread([&serverSocket, &client1Address, &client2Address]() {
		while(true)
		{
			SocketAddress incomingAddress;

			char buffer[4096];
			size_t nBytesReceived = serverSocket->ReceiveFrom(buffer, sizeof(buffer), incomingAddress);

			// if the server receives data, send it to all players
			if (nBytesReceived > 0)
			{
				// serverSocket->SendTo(buffer, nBytesReceived, incomingAddress);
				serverSocket->SendTo(buffer, nBytesReceived, *client1Address);
				serverSocket->SendTo(buffer, nBytesReceived, *client2Address);
			}
		}
	});

	serverThread.join();
}

// SDL requires this type of main function
int main(int argc, char* args[])
{
	SocketUtil::StaticInit();

	SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");

	char serverChoice;
	std::cout << "Type s to create a server.\nType c to create a client.\n> ";
	std::cin >> serverChoice;
	if (serverChoice == 's')
	{
		startServer(serverAddress);
	}
	else if (serverChoice != 'c')
	{
		close();
		SocketUtil::CleanUp();
		return 0;
	}

	char playerChoice;
	std::cout << "Type player number.\n1 if player one, 2 if player 2.\n> ";
	std::cin >> playerChoice;

	if (playerChoice != '1' && playerChoice != '2')
	{
		close();
		SocketUtil::CleanUp();
		return 0;
	}

	int playerNumber = playerChoice - 48;

	if (!init())
	{
		printf("init failure");
		close();
		SocketUtil::CleanUp();
		system("pause");
		return(0);
	}
	if (!loadMedia())
	{
		printf("loadMedia failure");
		close();
		SocketUtil::CleanUp();
		system("pause");
		return 0;
	}

	std::cout << playerChoice;

	UDPSocketPtr clientSocket = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);

	SocketAddressPtr clientAddr;
	if(playerChoice == '1')
		clientAddr = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8081");
	if(playerChoice == '2')
		clientAddr = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8082");

	std::cout << "\nClient address: " << clientAddr->ToString() << "\n";

	clientSocket->Bind(*clientAddr);

	bool quit = false;

	SDL_Event e;

	Color clientColor = Color(rand() % 155 + 100, rand() % 155 + 100, rand() % 155 + 100);
	player1.setColor(clientColor);
	crosshair1.setColor(clientColor);

	std::thread clientThread([&clientSocket, &playerChoice]() {
		while (true)
		{
			// read data from server
			SocketAddress incomingAddress;

			char buffer[4096];
			size_t nBytesReceived = clientSocket->ReceiveFrom(buffer, sizeof(buffer), incomingAddress);

			if (nBytesReceived > 0)
			{
				InputMemoryBitStream stream(buffer, static_cast<uint32_t>(nBytesReceived));
				char playerChoiceFromServer;
				char objectTypeFromServer;
				stream.Read(playerChoiceFromServer);
				stream.Read(objectTypeFromServer);
				if(playerChoiceFromServer != playerChoice)
				{
					// player
					if (objectTypeFromServer == 'P')
					{
						player2.ReadBits(stream);
					}
					// crosshair
					else if (objectTypeFromServer == 'C')
					{
						crosshair2.ReadBits(stream);
					}
					// bullets
					else if (objectTypeFromServer == 'B')
					{
						bullets2.clear();
						bullets2Size = 0;
						while (stream.GetRemainingBitCount() > 0)
						{
							Bullet bulletFromServer = Bullet();
							bulletFromServer.ReadBits(stream);
							bullets2.push_back(bulletFromServer);
							bullets2Size++;
						}
					}
				}
			}
		}
	});

	int sendInterval = 5;
	int waitToSend = sendInterval;
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			player1.handleEvent(e);
			crosshair1.handleEvent(e);

			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				Bullet bullet;
				bullet.setPosition(Vec2D(player1.getPosition().x + (player1.getSize().x / 2), player1.getPosition().y + (player1.getSize().y / 2)));
				bullet.setVelocity(Vec2D(-(player1.getVelocity().x / 2), -(player1.getVelocity().y / 2)));
				bullets1.push_back(bullet);
			}
		}

		// fill screen with SDL_SetRenderDrawColor
		SDL_RenderClear(gRenderer);

		// update textures
		player1.update();
		crosshair1.update();
		for (std::vector<Bullet>::iterator it = bullets1.begin(); it != bullets1.end(); ++it) {
			it->update();
		}

		// render textures
		player1.render();
		player2.render();
		crosshair1.render();
		crosshair2.render();
		for (std::vector<Bullet>::iterator it = bullets1.begin(); it != bullets1.end(); ++it) {
			it->render();
		}
		for (int i = 0; i < bullets2Size; i++)
		{
			if (bullets2Size == bullets2.size())
			{
				bullets2.at(i).render();
			}
		}

		std::vector<Bullet>::iterator it = bullets1.begin();
		while (it != bullets1.end())
		{
			if (it->getPosition().x < 0 || it->getPosition().y < 0 ||
				it->getPosition().x > SCREEN_WIDTH || it->getPosition().y > SCREEN_HEIGHT)
			{
				it = bullets1.erase(it);
			}
			else
			{
				++it;
			}
		}

		// update screen
		SDL_RenderPresent(gRenderer);

		waitToSend--;
		if (waitToSend < 0)
		{
			// send data to the server
			OutputMemoryBitStream stream1;
			stream1.Write(playerChoice);
			// player
			stream1.Write('P');
			player1.WriteBits(stream1);
			clientSocket->SendTo(stream1.GetBufferPtr(), stream1.GetBitLength(), *serverAddress);

			OutputMemoryBitStream stream2;
			stream2.Write(playerChoice);
			// crosshair
			stream2.Write('C');
			crosshair1.WriteBits(stream2);
			clientSocket->SendTo(stream2.GetBufferPtr(), stream2.GetBitLength(), *serverAddress);

			OutputMemoryBitStream stream3;
			stream3.Write(playerChoice);
			// bullets
			stream3.Write('B');
			for (std::vector<Bullet>::iterator it = bullets1.begin(); it != bullets1.end(); ++it) {
				it->WriteBits(stream3);
			}
			clientSocket->SendTo(stream3.GetBufferPtr(), stream3.GetBitLength(), *serverAddress);

			waitToSend = sendInterval;
		}
	}

	clientThread.join();
	close();
	SocketUtil::CleanUp();

	return 0;
}