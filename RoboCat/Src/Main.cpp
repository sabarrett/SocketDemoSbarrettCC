#pragma once
#include "RoboCatPCH.h"
#include "GraphicsSystems.h"
#include "InputSystem.h"
#include "GameObjects.h"
#include "Network.h"

//#pragma comment(lib, "SDL.lib")
#if _WIN32

const std::string ASSET_PATH = "Images\\";

//Network* ClientNetwork;
//Network* ServerNetwork;
Network* Networks;

GraphicsSystems* Graphics;
InputSystem* Inputs;


std::vector<ClassId> typesOfGameobjects;
std::vector<GameObject*> spawnedGameObjects;

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

	Network* ServerNetwork = new Network();
	ServerNetwork->init(Graphics, ASSET_PATH + "dean_spritesCropped.png", ASSET_PATH + "amongUs.png", ASSET_PATH + "SCOTT.png", connSocket);
	Networks = ServerNetwork;
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

	Network* ClientNetwork = new Network();
	ClientNetwork->init(Graphics, ASSET_PATH + "dean_spritesCropped.png", ASSET_PATH + "amongUs.png", ASSET_PATH + "SCOTT.png", clientSocket);
	Networks = ClientNetwork;
}

bool initGame()
{
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

	typesOfGameobjects.push_back(ClassId::DEANSPRITE);
	typesOfGameobjects.push_back(ClassId::AMONGUS);
	typesOfGameobjects.push_back(ClassId::SCOTTSPRITE);

	
	//Networks->init(Graphics, ASSET_PATH + "dean_spritesCropped.png", ASSET_PATH + "amongUs.png", ASSET_PATH + "SCOTT.png", );

	return didInIt;
}

void initCleanup()
{
	/*ClientNetwork->cleanUp();
	delete ClientNetwork;
	ClientNetwork = nullptr;

	ServerNetwork->cleanUp();
	delete ServerNetwork;
	ServerNetwork = nullptr;*/

	Networks->cleanUp();
	delete Networks;
	Networks = nullptr;

	Inputs->cleanup();
	delete Inputs;
	Inputs = nullptr;

	Graphics->cleanup();
	delete Graphics;
	Graphics = nullptr;
}

bool DrawRandomly(int pNetworkID, bool isServer)
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

				DeanSprite* Dean = new DeanSprite(pNetworkID, std::pair<float, float>(screenPosX, screenPosY), ASSET_PATH + "dean_spritesCropped.png", Graphics);
				Dean->Draw();
				spawnedGameObjects.push_back(Dean);

				/*if (isServer)
				{
					ClientNetwork->send(PacketType::CREATE_PACKET, Dean);
					ServerNetwork->receive();
				}
				else
				{
					ServerNetwork->send(PacketType::CREATE_PACKET, Dean);
					ClientNetwork->receive();
				}*/

				Networks->send(PacketType::CREATE_PACKET, Dean);

				break;
			}
			case AMONGUS:
			{
				int screenPosX = rand() % Graphics->getmScreenWidth() + 1;
				int screenPosY = rand() % Graphics->getmScreenHeight() + 1;
				AmongUs* Among = new AmongUs(pNetworkID, std::pair<float, float>(screenPosX, screenPosY), ASSET_PATH + "amongUs.png", Graphics);
				Among->Draw();
				spawnedGameObjects.push_back(Among);

				/*if (isServer)
				{
					ClientNetwork->send(PacketType::CREATE_PACKET, Among);
					ServerNetwork->receive();
				}
				else
				{
					ServerNetwork->send(PacketType::CREATE_PACKET, Among);
					ClientNetwork->receive();
				}*/

				Networks->send(PacketType::CREATE_PACKET, Among);

				break;
			}
			case SCOTTSPRITE:
			{
				int screenPosX = rand() % Graphics->getmScreenWidth() + 1;
				int screenPosY = rand() % Graphics->getmScreenHeight() + 1;
				ScottSprite* Scott = new ScottSprite(pNetworkID, std::pair<float, float>(screenPosX, screenPosY), ASSET_PATH + "SCOTT.png", Graphics);
				Scott->Draw();
				spawnedGameObjects.push_back(Scott);

				/*if (isServer)
				{
					ClientNetwork->send(PacketType::CREATE_PACKET, Scott);
					ServerNetwork->receive();
				}
				else
				{
					ServerNetwork->send(PacketType::CREATE_PACKET, Scott);
					ClientNetwork->receive();
				}*/

				Networks->send(PacketType::CREATE_PACKET, Scott);

				break;
			}
			}
		}
	}
	return false;
}

void DeleteRandomly(bool isServer)
{
	al_clear_to_color(al_map_rgba(0, 0, 0, 1));

	// Variables
	int imageToRemove;

	// Delete Object
	if (spawnedGameObjects.size() > 0)
	{
		imageToRemove = rand() % (spawnedGameObjects.size() + 0);
		//spawnedGameObjects.erase(spawnedGameObjects.begin() + imageToRemove);
		/*if (isServer)
		{
			ClientNetwork->send(PacketType::DELETE_PACKET, spawnedGameObjects[imageToRemove]);
			ServerNetwork->receive();
		}
		else
		{
			ServerNetwork->send(PacketType::DELETE_PACKET, spawnedGameObjects[imageToRemove]);
			ClientNetwork->receive();
		}*/

		Networks->send(PacketType::DELETE_PACKET, spawnedGameObjects[imageToRemove]);
	}

	// Redraw Scene
	if (spawnedGameObjects.size() == 0)
	{
		al_clear_to_color(al_map_rgba(0, 0, 0, 1));
		al_flip_display();
	}
	else
	{
		for (int i = 0; i < spawnedGameObjects.size(); i++)
		{
			spawnedGameObjects[i]->Draw();
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
	int networkID = 0;
	bool canSpawn = true;
	bool canDelete = true;
	bool run = true;

	while (run)
	{
		if (Inputs->getKeyState(KEYCODES::S) && canSpawn)
		{
			canSpawn = DrawRandomly(networkID,isServer);
			networkID++;
		}
		else if (!Inputs->getKeyState(KEYCODES::S))
		{
			canSpawn = true;
		}

		if (Inputs->getKeyState(KEYCODES::D) && canDelete)
		{
			DeleteRandomly(isServer);
			networkID--;
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

		Networks->receive();
	}

	initCleanup();
	SocketUtil::CleanUp();

	return 0;
}


