#pragma once
#pragma once

class GameObject;
#include "Client/GameObject.h"
#include "SDL.h"
#include "SDL_image.h"
#undef main

#include <string>
#include <vector>


class Server {

public:
	Server();
	~Server();

	// Core functions
	void Init();
	void Update();
	void CleanUp();

	GameObject* CreateObject(std::string name, int UID);
	void DestroyObject(GameObject* gameObject);

	// Get
	bool Running() { return m_isRunning; };
	const Uint8** getKeyStates() { return m_keyStates; }
private:
	bool m_isRunning;

	std::vector<GameObject*> m_allObjects;

	const Uint8* m_keyStates[8];

	int m_nextUID = 0;
};