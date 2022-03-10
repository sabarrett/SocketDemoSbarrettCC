#include "Game.h"
#include "Paddle.h"
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

Game::Game()
{
	

	if (InitAllegro())
	{
		ALLEGRO_COLOR paddleColor = al_map_rgb(255, 255, 255);
		mPaddleOne = new Paddle(20, 100, paddleColor);
		mPaddleOne->SetPosition(30, 100);

		mPaddleTwo = new Paddle(20, 100, paddleColor);
		mPaddleTwo->SetPosition(al_get_display_width(mDisplay) - 50, 100);

		keyboardState = new ALLEGRO_KEYBOARD_STATE();
		mRunning = true;
	}

	mIsServer = true;
	mLocalLeft = mIsServer;
	StartServer();

}

Game::Game(std::string IP)
{


	if (InitAllegro())
	{
		ALLEGRO_COLOR paddleColor = al_map_rgb(255, 255, 255);
		mPaddleOne = new Paddle(20, 100, paddleColor);
		mPaddleOne->SetPosition(30, 100);

		mPaddleTwo = new Paddle(20, 100, paddleColor);
		mPaddleTwo->SetPosition(al_get_display_width(mDisplay) - 50, 100);

		keyboardState = new ALLEGRO_KEYBOARD_STATE();
		mRunning = true;
	}
	mIsServer = false;
	mLocalLeft = mIsServer;
	ConnectToServer(IP);
}

Game::~Game()
{
	al_destroy_display(mDisplay);
	delete mPaddleOne, mPaddleTwo;
}

void Game::ProcessLocalInput()
{
	al_get_keyboard_state(keyboardState);

	if (al_key_down(keyboardState, ALLEGRO_KEY_SPACE))
	{
		mRunning = false;
	}

	if (al_key_down(keyboardState, ALLEGRO_KEY_TAB))
	{
		mLocalLeft = !mLocalLeft;
	}
	
	UpdateLocalPaddle();

}

void Game::UpdateLocalPaddle()
{
	int moveState = 0;

	if (al_key_down(keyboardState, ALLEGRO_KEY_UP))
	{
		moveState += 1;

		if (moveState > 1)
			moveState = 1;
	}

	if (al_key_down(keyboardState, ALLEGRO_KEY_DOWN))
	{
		moveState -= 1;

		if (moveState < -1)
			moveState = -1;
	}
	
	Paddle* localPaddle;
	
	if (mLocalLeft)
	{
		localPaddle = mPaddleOne;
	}
	else
	{
		localPaddle = mPaddleTwo;
	}

	
	int height = localPaddle->GetPosY();

	if (moveState == 1)
	{
		if (height > 0)
		{
			localPaddle->SetPosition(localPaddle->GetPosX(), height - 2);
		}
	}

	if (moveState == -1)
	{
		if (height + localPaddle->GetDimsW() < al_get_display_height(mDisplay))
		{
			localPaddle->SetPosition(localPaddle->GetPosX(), height + 2);
		}
	}
}

void Game::CheckCollisions()
{

}

void Game::Render()
{
	ALLEGRO_COLOR backgroundColor = al_map_rgb(0, 0, 0);
	al_clear_to_color(backgroundColor);

	mPaddleOne->Render();
	mPaddleTwo->Render();

	al_flip_display();
}

void Game::Update()
{
	while (mRunning)
	{

		ProcessLocalInput();
		Render();

		if (mIsServer)
		{
			CheckCollisions();
			//SendUpdatedStates();
		}
		else
		{
			//SendUpdatedStates();
		}
		
	}


}

bool Game::InitAllegro()
{
	//Init allegro
	if (!al_init())
	{
		std::cout << "error initting Allegro\n";
		system("pause");
		return false;
	}
	//Init font add on
	if (!al_init_font_addon())
	{
		std::cout << "error initting font add-on\n";
		system("pause");
		return false;
	}
	//Init ttf add on
	if (!al_init_ttf_addon())
	{
		std::cout << "error initting ttf add-on\n";
		system("pause");
		return false;
	}
	if (!al_init_primitives_addon())
	{
		std::cout << "error initting primitive add-on\n";
		system("pause");
		return false;
	}
	//Init keyboard
	if (!al_install_keyboard())
	{
		std::cout << "error installing Allegro keyboard plugin\n";
		system("pause");
		return false;
	}



	mDisplay = al_create_display(800, 640);

	return true;

}

void Game::StartServer()
{
	SocketUtil::StaticInit();


	// Create socket
	TCPSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (TCPSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}

	//listenSocket->SetNonBlockingMode(true);

	LOG("%s", "Listening socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:8080");
	if (listenAddress == nullptr)
	{
		SocketUtil::ReportError("Creating listen address");
		ExitProcess(1);
	}

	if (TCPSocket->Bind(*listenAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding listening socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound listening socket");

	// Blocking function call -> Waits for some input; halts the program until something "interesting" happens
	// Non-Blocking function call -> Returns right away, as soon as the action is completed

	// Listen() - Listen on socket -> Non-blocking; tells OS we care about incoming connections on this socket
	if (TCPSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on listening socket");
		ExitProcess(1);
	}

	LOG("%s", "Listening on socket");

	// Accept() - Accept on socket -> Blocking; Waits for incoming connection and completes TCP handshake

	LOG("%s", "Waiting to accept connections...");

	TCPSocket->SetNonBlockingMode(false);

	SocketAddress incomingAddress;
	TCPSocketPtr connSocket = TCPSocket->Accept(incomingAddress);
	
	TCPSocket->~TCPSocket();
	TCPSocket = connSocket;
	TCPSocket->SetNonBlockingMode(false);
	LOG("Accepted connection from %s", incomingAddress.ToString().c_str());


}

void Game::ConnectToServer(std::string ip)
{
	// Create socket
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
		return;
	}

	string address = "0.0.0.0:8080";
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError("Creating client address");
		ExitProcess(1);
		return;
	}

	if (clientSocket->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding client socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound client socket");

	// Connect() -> Connect socket to remote host

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString(ip);
	if (servAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	if (clientSocket->Connect(*servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}


	LOG("%s", "Connected to server!");

	
	clientSocket->SetNonBlockingMode(true);
}