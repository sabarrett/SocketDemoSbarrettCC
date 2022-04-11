#pragma once

#include "Util/RoboCatPCH.h"
#include "Util/NetworkEnums.h"
#include "Client/NetworkManagerClient.h"
#include "SDL.h"
#include "SDL_image.h"
#undef main

#include <string>
#include <vector>
#include <queue>

class GameObject;

struct Inputs
{
	const Uint8* m_keyStates;
	int m_moveInput;
	int m_mouseX;
	int m_mouseY;
	bool m_leftClickDown;
};

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

	// Connect
	void ConnectToServer(std::string address, std::string port);

	// General
	void DestroyObject(GameObject* gameObject);

	// Get
	bool Running() { return m_isRunning; };
	//const Uint8* getKeyStates() { return m_keyStates; }
	Inputs* GetInput() { return &m_input; }
	SDL_Renderer* GetRenderer() { return m_renderer; }

	// Set
	void CopyNetworkObjectsVector(std::vector<GameObject*> inAllObjects) { m_allObjects = inAllObjects; };

	// Network Simulation
	void SetLatency(float latency) { m_networkManager.SetSimulatedLatency(latency); }
	void SetJitter(float jitter) { m_networkManager.SetSimulatedJitter(jitter); }
	void SetDrop(float drop) { m_networkManager.SetDropPacketChance(drop); }

private:
	// Game
	bool m_isRunning;
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	SDL_Texture* m_textures[8] = { 0 }; // - Max is 8, so texture indexes are stored as uint8
	std::vector<GameObject*> m_allObjects;

	// Input;
	Inputs m_input;
	
	NetworkManagerClient m_networkManager;
};