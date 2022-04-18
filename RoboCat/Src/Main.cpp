#pragma once
#include "RoboCatPCH.h"
#include "GraphicsSystems.h"
#include "InputSystem.h"
#include "GameObjects.h"
#include "Network.h"

#if _WIN32

const std::string ASSET_PATH = "Images\\";

Network* ClientNetwork;
Network* ServerNetwork;

int mNetworkID = 0;

GraphicsSystems* Graphics;
InputSystem* Inputs;
//DeliveryNotificationManager* DeliveryManager; ASSIGNMENT 3

std::vector<ClassId> typesOfGameobjects;

int ScreenSizeX = 800;
int ScreenSizeY = 600;

void DoTcpServer(std::string port)
{
	// Create socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}

	LOG("%s", "Listening socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:8080");
	if (listenAddress == nullptr)
	{
		SocketUtil::ReportError("Creating listen address");
		ExitProcess(1);
	}

	if (listenSocket->Bind(*listenAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding listening socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound listening socket");

	// Listen() - Listen on socket -> Non-blocking; tells OS we care about incoming connections on this socket
	if (listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on listening socket");
		ExitProcess(1);
	}

	LOG("%s", "Listening on socket");

	// Accept() - Accept on socket -> Blocking; Waits for incoming connection and completes TCP handshake

	LOG("%s", "Waiting to accept connections...");
	SocketAddress incomingAddress;
	TCPSocketPtr connSocket = listenSocket->Accept(incomingAddress);
	while (connSocket == nullptr)
	{
		connSocket = listenSocket->Accept(incomingAddress);
	}

	LOG("Accepted connection from %s", incomingAddress.ToString().c_str());

	connSocket->SetNonBlockingMode(true);

	ServerNetwork = new Network();
	ServerNetwork->init(Graphics, /*DeliveryManager,*/ ASSET_PATH + "dean_spritesCropped.png", ASSET_PATH + "amongUs.png", ASSET_PATH + "SCOTT.png", connSocket);
}

void DoTcpClient(std::string port)
{
	// Create socket
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
	}

	LOG("%s", "Client socket created");

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	std::string address = StringUtils::Sprintf("127.0.0.1:%s", port.c_str());
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
	{
		SocketUtil::ReportError("Creating client address");
		ExitProcess(1);
	}

	if (clientSocket->Bind(*clientAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding client socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound client socket");

	// Connect() -> Connect socket to remote host

	SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
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

	ClientNetwork = new Network();
	ClientNetwork->init(Graphics, /*DeliveryManager,*/ ASSET_PATH + "dean_spritesCropped.png", ASSET_PATH + "amongUs.png", ASSET_PATH + "SCOTT.png", clientSocket);
}

bool initGame()
{
	srand(time(0));

	bool didInIt = false;

	Graphics = new GraphicsSystems;
	didInIt = Graphics->init(ScreenSizeX, ScreenSizeY);

	if (!didInIt)
	{
		return false;
	}

	Inputs = new InputSystem;
	didInIt = Inputs->initInputSystem(Graphics);

	if (!didInIt)
	{
		return false;
	}

	// ASSIGNMENT 3
	//DeliveryManager = new DeliveryNotificationManager(true, true);
	//didInIt = DeliveryManager;

	//if (!didInIt)
	//{
	//	return false;
	//}

	typesOfGameobjects.push_back(ClassId::DEANSPRITE);
	typesOfGameobjects.push_back(ClassId::AMONGUS);
	typesOfGameobjects.push_back(ClassId::SCOTTSPRITE);

	return didInIt;
}

void initCleanup()
{
	if (ClientNetwork)
	{
		ClientNetwork->cleanUp();
		delete ClientNetwork;
		ClientNetwork = nullptr;
	}

	if (ServerNetwork)
	{
		ServerNetwork->cleanUp();
		delete ServerNetwork;
		ServerNetwork = nullptr;
	}

	Inputs->cleanup();
	delete Inputs;
	Inputs = nullptr;

	Graphics->cleanup();
	delete Graphics;
	Graphics = nullptr;
}

bool DrawRandomly(bool isServer)
{
	int spriteToSpawn = rand() % typesOfGameobjects.size() + 0;
	for (int i = 0; i < typesOfGameobjects.size(); i++)
	{
		if (i == spriteToSpawn)
		{
			switch (typesOfGameobjects[i])
			{
			case DEANSPRITE:
			{
				int screenPosX = rand() % Graphics->getmScreenWidth() + 1;
				int screenPosY = rand() % Graphics->getmScreenHeight() + 1;

				mNetworkID++;
				DeanSprite* Dean = new DeanSprite(mNetworkID, std::pair<float, float>(screenPosX, screenPosY), ASSET_PATH + "dean_spritesCropped.png", Graphics);
				Dean->Draw();

				if (isServer)
				{
					ServerNetwork->send(PacketType::CREATE_PACKET, Dean);
				}
				else
				{
					ClientNetwork->send(PacketType::CREATE_PACKET, Dean);
				}
				break;
			}
			case AMONGUS:
			{
				int screenPosX = rand() % Graphics->getmScreenWidth() + 1;
				int screenPosY = rand() % Graphics->getmScreenHeight() + 1;

				mNetworkID++;
				AmongUs* Among = new AmongUs(mNetworkID, std::pair<float, float>(screenPosX, screenPosY), ASSET_PATH + "amongUs.png", Graphics);
				Among->Draw();

				if (isServer)
				{
					ServerNetwork->send(PacketType::CREATE_PACKET, Among);
				}
				else
				{
					ClientNetwork->send(PacketType::CREATE_PACKET, Among);
				}

				break;
			}
			case SCOTTSPRITE:
			{
				int screenPosX = rand() % Graphics->getmScreenWidth() + 1;
				int screenPosY = rand() % Graphics->getmScreenHeight() + 1;

				mNetworkID++;
				ScottSprite* Scott = new ScottSprite(mNetworkID, std::pair<float, float>(screenPosX, screenPosY), ASSET_PATH + "SCOTT.png", Graphics);
				Scott->Draw();

				if (isServer)
				{
					ServerNetwork->send(PacketType::CREATE_PACKET, Scott);
				}
				else
				{
					ClientNetwork->send(PacketType::CREATE_PACKET, Scott);
				}
				break;
			}
			}
		}
	}
	return false;
}

void DeleteRandomly(bool isServer)
{
	// Delete object
	if (mNetworkID > 0)
	{
		if (isServer)
		{
			ServerNetwork->send(PacketType::DELETE_PACKET, ServerNetwork->getmGameObjects().back().second);
			mNetworkID--;
		}
		else
		{
			ClientNetwork->send(PacketType::DELETE_PACKET, ClientNetwork->getmGameObjects().back().second);
			mNetworkID--;
		}
	}
}


#define SDL_MAIN_HANDLED
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
	// Server or Client
	//bool isServer = true;
	bool isServer = StringUtils::GetCommandLineArg(1) == "server";

	// Init Game
	initGame();

	// Connect to Server
	if (isServer)
	{
		// Server code ----------------
		DoTcpServer(StringUtils::GetCommandLineArg(2));
	}
	else
	{
		// Client code ----------------
		DoTcpClient(StringUtils::GetCommandLineArg(2));
	}

	// Game Loop
	
	bool canSpawn = true;
	bool canDelete = true;
	bool run = true;

	while (run)
	{
		if (Inputs->getKeyState(KEYCODES::S) && canSpawn)
		{
			canSpawn = DrawRandomly(isServer);
		}
		else if (!Inputs->getKeyState(KEYCODES::S))
		{
			canSpawn = true;
		}

		if (Inputs->getKeyState(KEYCODES::D) && canDelete)
		{
			DeleteRandomly(isServer);
			canDelete = false;
		}
		else if (!Inputs->getKeyState(KEYCODES::D))
		{
			canDelete = true;
		}

		if (Inputs->getKeyState(KEYCODES::Esc))
		{
			run = false;
		}

		if (isServer)
		{
			ServerNetwork->receive();

			// Redraw Scene
			al_clear_to_color(al_map_rgba(0, 0, 0, 1));
			if (ServerNetwork->getmGameObjects().size() > 0)
			{
				for (int i = 0; i < ServerNetwork->getmGameObjects().size(); i++)
				{
					ServerNetwork->getmGameObjects()[i].second->Draw();
				}
			}
		}
		else
		{
			ClientNetwork->receive();

			// Redraw Scene
			al_clear_to_color(al_map_rgba(0, 0, 0, 1));
			if (ClientNetwork->getmGameObjects().size() > 0)
			{
				for (int i = 0; i < ClientNetwork->getmGameObjects().size(); i++)
				{
					ClientNetwork->getmGameObjects()[i].second->Draw();
				}
			}
		}

		//DeliveryManager->ProcessTimedOutPackets(); ASSIGNMENT 3
		al_flip_display();
	}

	initCleanup();
	SocketUtil::CleanUp();

	return 0;
}


