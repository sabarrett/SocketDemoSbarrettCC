#pragma once

#include "Util/RoboCatPCH.h"
#include "Util/NetworkEnums.h"
#include "SDL.h"
#include "SDL_image.h"
#undef main

#include <string>
#include <vector>

class GameObject;

class Game {

public:
	Game() {};
	~Game() {};

	// Core functions
	void Init(char* title, int width, int height);
	void Update();
	void Draw();
	void DoNetworking();
	void CleanUp();

	// Inits
	void CreateGameWindow(char* title, int width, int height);
	void LoadTexture(std::string assetPath);

	// General
	GameObject* CreateObject(int type, int UID, uint8_t textureID);
	GameObject* GetObjectByUID(int UID);
	void DestroyObject(GameObject* gameObject);
	bool CheckIfObjectExistsWithUID(int UID);

	// Get
	bool Running() { return m_isRunning; };
	const Uint8* getKeyStates() { return m_keyStates; }
	SDL_Renderer* getRenderer() { return m_renderer; }

	// Set
	void setPlayerID(int UID) { m_myPlayerUID = UID; }

	// Networking
	void ConnectToServer(std::string address, std::string port);
private:
	void ReceiveTCP(); // Handle in data from TCP socket -> Handshake, playerID assignment
	void ReceiveUDP(); // Handle in data from UDP socket -> World State Updates
	void HandleWorldStatePacket(InputMemoryBitStream* inStream);

	// Game
	bool m_isRunning;
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	SDL_Texture* m_textures[8] = { 0 }; // - Max is 8, so texture indexes are stored as uint8
	std::vector<GameObject*> m_allObjects;

	// Input;
	const Uint8* m_keyStates;
	int m_moveInput;
	int m_mouseX;
	int m_mouseY;
	bool m_leftClickDown;
	int m_myPlayerUID; // UID of the object move commands from this client will target

	// Networking
	TCPSocketPtr clientSocketTCP;
	UDPSocketPtr clientSocketUDP;
	SocketAddressPtr serverAddress;
};