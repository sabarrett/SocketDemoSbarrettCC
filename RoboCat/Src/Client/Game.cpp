#include "Client/Game.h"
#include "Client/GameObject.h"
#include "Client/ColorChangerGameObject.h"
#include "Client/CollectableGameObject.h"

#include <iostream>

void Game::Init(char* title, int width, int height)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	CreateGameWindow(title, width, height);
	m_isRunning = true;

	LoadTexture("Assets/Player.png");
	LoadTexture("Assets/PlayerScheming.png");
	LoadTexture("Assets/PlayerRed.png");
	LoadTexture("Assets/PlayerGreen.png");
	LoadTexture("Assets/PlayerBlue.png");
	LoadTexture("Assets/Cat.png");
	LoadTexture("Assets/Coin.png");
	LoadTexture("Assets/Background.png");
}
void Game::CreateGameWindow(char* title, int width, int height)
{
	m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
	m_renderer = SDL_CreateRenderer(m_window, -1, 0);

	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
}

void Game::LoadTexture(std::string assetPath)
{
	int i = 0;
	while (m_textures[i] != 0)
	{
		i++;
	}
	SDL_Surface* tmpSurface = IMG_Load(assetPath.c_str());
	m_textures[i] = SDL_CreateTextureFromSurface(m_renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);
}

void Game::Update()
{
	// Handle events
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

	// Handle keyboard input
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
	
	// Handle mouse input
	m_leftClickDown = false;
	if (SDL_GetMouseState(&m_mouseX, &m_mouseY) == SDL_BUTTON_LMASK)
	{
		m_leftClickDown = true;
	}
	
	// Delete want-to-die objects
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		if (m_allObjects[i]->GetShouldDestroy())
		{
			m_allObjects[i]->CleanUp();
			m_allObjects.erase(m_allObjects.begin() + i);
		}
	}
}

void Game::DoNetworking()
{
	ReceiveTCP(); // Check TCP socket for data
	ReceiveUDP(); // Check UDP socket for data -> WORLD UPDATES

	// Send move update
	if (m_moveInput != 0)
	{
		OutputMemoryBitStream inputPacket;
		uint32_t OutPacketType = PKTTYPE_MOVEOBJECT;
		inputPacket.Write(OutPacketType);
		inputPacket.Write(m_myPlayerUID);
		inputPacket.Write(m_moveInput);
		clientSocketUDP->SendTo(inputPacket.GetBufferPtr(), inputPacket.GetByteLength(), *serverAddress);
	}

	// Send create object update
	if (m_leftClickDown)
	{
		OutputMemoryBitStream inputPacket;
		uint32_t OutPacketType = PKTTYPE_CREATEOBJECT;
		uint8_t textureID = 6;
		inputPacket.Write(OutPacketType);
		inputPacket.Write(textureID);
		inputPacket.Write(m_mouseX);
		inputPacket.Write(m_mouseY);
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
		case PKTTYPE_WORLDSTATE: 
			HandleWorldStatePacket(&inStream);
			break;
		}
	}
}
void Game::HandleWorldStatePacket(InputMemoryBitStream* inStream)
{
	// Set objects to be destroyed if not contained in the following update
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		m_allObjects[i]->SetShouldDestroy(true); 
	}

	// Packets always start with these three:
	int totalObjects;
	int UID;
	uint8_t objectType;
	inStream->Read(totalObjects);
	for (int i = 0; i < totalObjects; i++)
	{
		inStream->Read(UID);
		inStream->Read(objectType);
		switch (objectType)
		{
			case GAMEOBJECT_BASE:
			{
				GameObject* targetObject = GetObjectByUID(UID);
				if (targetObject == nullptr)
				{
					targetObject = CreateObject(objectType, UID, 0);
				}
				targetObject->Read(inStream);
				targetObject->SetShouldDestroy(false);
				break;
			}
			case GAMEOBJECT_COLORCHANGER:
			{
				ColorChangerGameObject* targetObject = (ColorChangerGameObject*)GetObjectByUID(UID);
				if (targetObject == nullptr)
				{
					targetObject = (ColorChangerGameObject*)CreateObject(objectType, UID, 0);
				}
				// Where other objects are very simple to read and write,
				// due to the need to getObjects by UID, this object's read is handled specially here.
				bool readMore;
				inStream->Read(readMore);
				if (readMore)
				{
					targetObject->Read(inStream);
					size_t count = 0;
					inStream->Read(count);
					for (size_t j = 0; j < count; j++)
					{
						int targetUID;
						inStream->Read(targetUID);
						targetObject->AddTargetObject(GetObjectByUID(targetUID));
					}
				}
				targetObject->SetShouldDestroy(false);
				break;
			}
			case GAMEOBJECT_COLLECTABLE:
			{
				CollectableGameObject* targetObject = (CollectableGameObject*)GetObjectByUID(UID);
				if (targetObject == nullptr)
				{
					targetObject = (CollectableGameObject*)CreateObject(objectType, UID, 0);
				}
				targetObject->Read(inStream);
				targetObject->SetShouldDestroy(false);
				break;
			}
		}
	}
}

void Game::Draw()
{
	// Clear + background
	SDL_RenderClear(m_renderer);
	SDL_RenderCopy(m_renderer, m_textures[7], NULL, NULL);

	// Draw all objects
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

GameObject* Game::CreateObject(int type, int UID, uint8_t textureID)
{
	GameObject* gameObject;
	switch (type)
	{
	case GAMEOBJECT_BASE:
		gameObject = new GameObject;
		break;
	case GAMEOBJECT_COLORCHANGER:
		gameObject = new ColorChangerGameObject;
		break;
	case GAMEOBJECT_COLLECTABLE:
		gameObject = new CollectableGameObject;
		break;
	default:
		gameObject = new GameObject;
		break;
	}

	gameObject->Init(UID, textureID);
	m_allObjects.push_back(gameObject);
	return gameObject;
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


