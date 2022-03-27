#include "Client/Game.h"
#include "Client/GameObject.h"
#include "Client/ArrayOfPointersGameObject.h"

#include <iostream>

Game::Game()
{
}

Game::~Game()
{
}

void Game::Init(char* title, int width, int height)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	CreateGameWindow(title, width, height);
	m_isRunning = true;

	SDL_Surface* tmpSurface = IMG_Load("Assets/Player.png");
	printf(" %s\n", SDL_GetError());
	m_textures[0] = SDL_CreateTextureFromSurface(m_renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load("Assets/PlayerScheming.png");
	printf(" %s\n", SDL_GetError());
	m_textures[1] = SDL_CreateTextureFromSurface(m_renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load("Assets/PlayerRed.png");
	printf(" %s\n", SDL_GetError());
	m_textures[2] = SDL_CreateTextureFromSurface(m_renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load("Assets/PlayerGreen.png");
	printf(" %s\n", SDL_GetError());
	m_textures[3] = SDL_CreateTextureFromSurface(m_renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load("Assets/PlayerBlue.png");
	printf(" %s\n", SDL_GetError());
	m_textures[4] = SDL_CreateTextureFromSurface(m_renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	tmpSurface = IMG_Load("Assets/Cat.png");
	printf(" %s\n", SDL_GetError());
	m_textures[5] = SDL_CreateTextureFromSurface(m_renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);

	//CreateObject(0, 0);
}

void Game::CreateGameWindow(char* title, int width, int height)
{
	m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	m_renderer = SDL_CreateRenderer(m_window, -1, 0);

	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
}

void Game::Update()
{
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent) != 0)
	{
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
			m_isRunning = false;
			break;
		default:
			break;
		}
	}
	m_keyStates = SDL_GetKeyboardState(NULL);
	m_moveInput = 0;
	if (m_keyStates[SDL_SCANCODE_W])
	{
		m_moveInput += MOVE_UP;
	}
	if (m_keyStates[SDL_SCANCODE_S])
	{
		m_moveInput += MOVE_DOWN;
	}
	if (m_keyStates[SDL_SCANCODE_A])
	{
		m_moveInput += MOVE_LEFT;
	}
	if (m_keyStates[SDL_SCANCODE_D])
	{
		m_moveInput += MOVE_RIGHT;
	}

	

	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		if (m_allObjects[i]->GetShouldDestroy())
		{
			m_allObjects.erase(m_allObjects.begin() + i);
		}
	}
}

void Game::DoNetworking()
{
	ReceiveUDP();
	ReceiveTCP();

	if (m_moveInput != 0)
	{
		uint32_t OutPacketType = PKTTYPE_MOVEOBJECT;
		OutputMemoryBitStream inputPacket;
		inputPacket.Write(OutPacketType);
		inputPacket.Write(m_myPlayerUID); // TARGET OBJECT ID
		inputPacket.Write(m_moveInput);
		clientSocketUDP->SendTo(inputPacket.GetBufferPtr(), inputPacket.GetByteLength(), *serverAddress);
	}

	int mouseX;
	int mouseY;
	if (SDL_GetMouseState(&mouseX, &mouseY) == SDL_BUTTON_LMASK)
	{
		uint32_t OutPacketType = PKTTYPE_CREATEOBJECT;
		OutputMemoryBitStream inputPacket;
		inputPacket.Write(OutPacketType);
		inputPacket.Write(0);
		inputPacket.Write(mouseX);
		inputPacket.Write(mouseY);
		clientSocketUDP->SendTo(inputPacket.GetBufferPtr(), inputPacket.GetByteLength(), *serverAddress);
	}
}

void Game::ReceiveTCP()
{
	char buffer[4096];
	int32_t bytesReceived = 1;
	while (bytesReceived > 0)
	{
		bytesReceived = clientSocketTCP->Receive(buffer, 4096);

		InputMemoryBitStream inStream(buffer, bytesReceived);
		uint32_t packetType;
		inStream.Read(packetType);
		switch (packetType)
		{
		case PKTTYPE_SETPLAYERID: // MOVE
			int UID;
			inStream.Read(UID);
			m_myPlayerUID = UID;
			std::cout << "\n\n\n" << m_myPlayerUID;
			break;
		}
	}
}

void Game::ReceiveUDP()
{
	char buffer[8192];
	int32_t bytesReceived = 1;
	while (bytesReceived > 0)
	{
		SocketAddress fromAddr;
		bytesReceived = clientSocketUDP->ReceiveFrom(buffer, 8192, fromAddr);

		InputMemoryBitStream inStream(buffer, bytesReceived);
		uint32_t packetType;
		inStream.Read(packetType);
		switch (packetType)
		{
		case PKTTYPE_WORLDSTATE: // MOVE
			int totalObjects;
			int UID;
			uint8_t objectType;
			inStream.Read(totalObjects);
			for (int i = 0; i < totalObjects; i++)
			{
				inStream.Read(UID);
				inStream.Read(objectType);
				if (objectType == 0)
				{
					GameObject* targetObject = GetObjectByUID(UID);
					if (targetObject == nullptr)
					{
						targetObject = CreateObject(0, UID, 0);
					}
					targetObject->Read(&inStream);
					targetObject->SetShouldDestroy(false);
				}
				if (objectType == 1)
				{
					ArrayOfPointersGameObject* targetObject = (ArrayOfPointersGameObject*)GetObjectByUID(UID);
					if (targetObject == nullptr)
					{
						targetObject = (ArrayOfPointersGameObject*)CreateObject(1 , UID, 0);
					}
					bool readMore;
					inStream.Read(readMore);
					if (readMore)
					{
						targetObject->Read(&inStream);
						size_t count = 0;
						inStream.Read(count);
						for (size_t j = 0; j < count; j++)
						{
							int targetUID;
							inStream.Read(targetUID);
							targetObject->AddTargetObject(GetObjectByUID(targetUID));
						}
					}
					targetObject->SetShouldDestroy(false);
				}
				
			}
			break;
		}
	}
}

void Game::Draw()
{
	SDL_RenderClear(m_renderer);
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		int texID = m_allObjects[i]->GetTextureID();
		m_allObjects[i]->Draw(m_renderer, m_textures[texID]);
	}
	SDL_RenderPresent(m_renderer);
}

void Game::ConnectToServer(std::string address, std::string port)
{
	// Create socket
	clientSocketTCP = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	clientSocketUDP = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);

	if (clientSocketTCP == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
	}

	// Bind() - "Bind" socket -> bind to address:port - trying ports 64000-64100
	bool socketBound = false;
	int localPort = 64000;
	while (!socketBound && localPort <= 64100)
	{
		std::string myIPAddr = StringUtils::Sprintf("%s:%d", "127.0.0.1", localPort);
		SocketAddressPtr clientTCPAddress = SocketAddressFactory::CreateIPv4FromString(myIPAddr);
		if (clientTCPAddress == nullptr)
		{
			SocketUtil::ReportError("Creating client address");
		}
		if (clientSocketTCP->Bind(*clientTCPAddress) != NO_ERROR)
		{
			SocketUtil::ReportError("Binding TCP client socket");
			localPort++;
		}
		else
		{
			socketBound = true;
			LOG("Bound TCP client socket to %s", myIPAddr.c_str());
		}
	}
	socketBound = false;
	while (!socketBound && localPort <= 64100)
	{
		std::string myIPAddr = StringUtils::Sprintf("%s:%d", "127.0.0.1", localPort);
		SocketAddressPtr clientUDPAddress = SocketAddressFactory::CreateIPv4FromString(myIPAddr);
		if (clientUDPAddress == nullptr)
		{
			SocketUtil::ReportError("Creating client address");
		}
		if (clientSocketUDP->Bind(*clientUDPAddress) != NO_ERROR)
		{
			SocketUtil::ReportError("Binding UDP client socket");
			localPort++;
		}
		else
		{
			socketBound = true;
			clientSocketUDP->SetNonBlockingMode(true);
			LOG("Bound UDP client socket to %s", myIPAddr.c_str());
		}
	}

	// Connect() -> Connect socket to remote host
	std::string serverAddressString = StringUtils::Sprintf("%s:%s", address.c_str(), port.c_str());
	serverAddress = SocketAddressFactory::CreateIPv4FromString(serverAddressString);
	if (serverAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}
	if (clientSocketTCP->Connect(*serverAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}
	LOG("%s", "Connected to server!");
	clientSocketTCP->SetNonBlockingMode(true);
}

bool Game::CheckIfObjectExistsWithUID(int UID)
{
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		if (m_allObjects[i]->GetUID() == UID)
		{
			return true;
		}
	}
	return false;
}

GameObject* Game::GetObjectByUID(int UID)
{
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		if (m_allObjects[i]->GetUID() == UID)
		{
			return m_allObjects[i];
		}
	}
	return nullptr;
}

GameObject* Game::CreateObject(int type, int UID, int textureID)
{
	if (type == 0)
	{
		GameObject* gameObject = new GameObject;
		gameObject->Init(UID, textureID);
		m_allObjects.push_back(gameObject);
		return gameObject;
	}
	if (type == 1)
	{
		ArrayOfPointersGameObject* gameObject = new ArrayOfPointersGameObject;
		gameObject->Init(UID, textureID);
		m_allObjects.push_back(gameObject);
		return gameObject;
	}
}

void Game::DestroyObject(GameObject* gameObject)
{
	gameObject->CleanUp();
}

void Game::CleanUp()
{
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		DestroyObject(m_allObjects[i]);
	}
	SDL_DestroyWindow(m_window);
	SDL_DestroyRenderer(m_renderer);
	SDL_Quit();
}


