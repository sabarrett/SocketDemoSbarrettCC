#pragma once

class GameObject;
//#include "GameObject.h"
#include "Util/RoboCatPCH.h"
#include "Util/NetworkEnums.h"
#include "SDL.h"
#include "SDL_image.h"
#undef main

#include <string>
#include <vector>


class Game {

public:
	Game();
	~Game();

	// Core functions
	void Init(char* title, int width, int height);
	void Update();
	void Draw();
	void DoNetworking();
	void CleanUp();

	GameObject* CreateObject(int type, int UID, int textureID);
	void DestroyObject(GameObject* gameObject);


	void CreateGameWindow(char* title, int width, int height);

	void ConnectToServer(std::string address, std::string port);

	bool CheckIfObjectExistsWithUID(int UID);
	GameObject* GetObjectByUID(int UID);

	// Get
	bool Running() { return m_isRunning; };
	const Uint8* getKeyStates() { return m_keyStates; }
	SDL_Renderer* getRenderer() { return m_renderer; }
	void setPlayerID(int UID) { m_myPlayerUID = UID; }
private:
	
	void ReceiveTCP();
	void ReceiveUDP();
	void SendTCP();

	bool m_isRunning;
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	SDL_Texture* m_textures[8];
	std::vector<GameObject*> m_allObjects;

	const Uint8* m_keyStates;
	int m_moveInput;
	int m_myPlayerUID;

	TCPSocketPtr clientSocketTCP;
	UDPSocketPtr clientSocketUDP;
	SocketAddressPtr serverAddress;

	int m_nextUID = 0;
};