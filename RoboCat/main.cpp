#include <RoboCatPCH.h>
#include "SDL.h"
#include <SDL_image.h>
#undef main
#include "Unit.h"
#include "UnitManager.h"
#include <iostream>
#include <thread>
#include "PlayerUser.h"

PlayerUser* p1, * p2;

bool isRunning;
bool fullscreen;

void handleEvents();
void update(float dt);
void render();
void delaySend(int _id, PlayerUser* user, int flag);

#define TICK_INTERVAL    30

static Uint32 next_time;


Uint32 time_left(void)
{
	Uint32 now;

	now = SDL_GetTicks();
	if (next_time <= now)
		return 0;
	else
		return next_time - now;
}

std::string CLIENT_SEND_PORT = "1250", CLIENT_RECV_PORT = "2250";

std::thread sendThread1, sendThread2, delThread1, delThread2;

#if _WIN32
int main(int argc, const char** argv)
{
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);
#else
const char** __argv;
int __argc;
int main(int argc, const char** argv, const char** argz)
{
	__argc = argc;
	__argv = argv;
	__argz = argz;
#endif

	SocketUtil::StaticInit();


	fullscreen = false;
	int flags = 0;
	flags = SDL_WINDOW_RESIZABLE;
	if (fullscreen) {
		flags = flags | SDL_WINDOW_FULLSCREEN;
	}
	
	p1 = new PlayerUser(flags, 1, 100);
	p2 = new PlayerUser(flags, 2, 1300);

	CLIENT_SEND_PORT = std::to_string(rand() % 1000 + 8999);
	CLIENT_RECV_PORT = std::to_string(rand() % 1000 + 8999);

	OutputWindow p1Client,p2Client;
	std::thread p1c([&p1Client]()
	{	
		p1->initTcpClient(CLIENT_SEND_PORT, CLIENT_RECV_PORT);
	});
	std::thread p2c([&p2Client]()
	{
		p2->initTcpServer(CLIENT_RECV_PORT);
	});


	Uint32 lastUpdate = SDL_GetTicks();

	isRunning = true;

	while (isRunning) 
	{
		// for physics loop
		Uint32 current = SDL_GetTicks();
		float dt = (current - lastUpdate) / 1000.0f;

		handleEvents();
		update(dt);
		render();

		SDL_Delay(time_left());
		next_time += TICK_INTERVAL;
		lastUpdate = current;
	}

	//frees memory associated with renderer and window


	p1->shutdown();
	p1c.join();

	p2->shutdown();
	p2c.join();

	delete p2;
	delete p1;
	SocketUtil::CleanUp();
	SDL_Quit();

	return 0;
}

void handleEvents() 
{
	SDL_Event event;
	SDL_PollEvent(&event);


	switch (event.type) 
	{
	case SDL_QUIT:
		isRunning = false;
		std::system("Pause");
		break;
	case SDL_KEYDOWN:
	{
		bool isP1Window = event.window.windowID == SDL_GetWindowID(p1->window);
		PlayerUser* eventUser = (isP1Window ? p1 : p2);

		if (event.key.keysym.sym == SDLK_j)
		{
			if (sendThread1.joinable())
				sendThread1.join();
			else if (sendThread2.joinable())
				sendThread2.join();
			else
				std::cout << "WAITING FOR THREAD...\n";

			std::cout << "Create rand unit here" << " FROM " << (eventUser->playerName) << std::endl;

			int id = eventUser->createRandomUnit();
			if (id != -1)
				(sendThread1.joinable() ? sendThread2 : sendThread1) = std::thread(delaySend, id, eventUser, 0);		
				//eventUser->sendUnitIterator(id);
		}
		else if (event.key.keysym.sym == SDLK_k)
		{
			if (delThread1.joinable())
				delThread1.join();
			else if (delThread2.joinable())
				delThread2.join();
			else 
				std::cout << "WAITING FOR THREAD...\n";

			std::cout << "Delete rand unit here" << " FROM " << (eventUser->playerName) << std::endl;

			int id = eventUser->deleteRandomUnit();
			if (id != -1)
			{
				(delThread1.joinable() ? delThread2 : delThread1) = std::thread(delaySend, id, eventUser, 1);
				eventUser->unitManager.deleteUnit(id);
			}
		}
		else if (event.key.keysym.sym == SDLK_ESCAPE)
			isRunning = false;
	}
	default:
		break;
	}
}

void render() 
{
	SDL_SetRenderDrawColor(p1->renderer, 121, 121, 121, 255);
	SDL_RenderClear(p1->renderer);
	SDL_RenderPresent(p1->renderer);
	p1->unitManager.RenderUnits(p1->renderer);

	SDL_SetRenderDrawColor(p2->renderer, 121, 121, 121, 255);
	SDL_RenderClear(p2->renderer);
	SDL_RenderPresent(p2->renderer);
	p2->unitManager.RenderUnits(p2->renderer);
}

//simple update function
void update(float dt) 
{
	int w, h;
	SDL_GetWindowSize(p1->window, &w, &h);
	p1->unitManager.updateUnits(dt, Vector2(w, h));

	SDL_GetWindowSize(p2->window, &w, &h);
	p2->unitManager.updateUnits(dt, Vector2(w, h));
}

void delaySend(int _id, PlayerUser* user, int flag)
{
	srand(_id);
	int delay = std::rand() % 1000;
	int shouldDrop = std::rand() % 6 + 1;
	if (shouldDrop != 5)
	{
		std::cout << "Lagging for " << delay << " ms while sending data\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(delay));
		flag == 0 ? user->sendUnitIterator(_id) : user->sendUnitDelete(_id);
	}
	else
	{
		while (shouldDrop == 5)
		{
			std::cout << "Dropped! Waiting one second before trying again...\n";
			shouldDrop = std::rand() % 10 + 1;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			std::cout << "Trying again...\n";
		}
		flag == 0 ? user->sendUnitIterator(_id) : user->sendUnitDelete(_id);
	}
}

