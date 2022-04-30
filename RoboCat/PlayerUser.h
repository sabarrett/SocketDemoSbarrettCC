#pragma once

#include "RoboCatPCH.h"
#include <SDL.h>
#include "UnitManager.h"

struct PlayerUser
{
	SDL_Renderer* renderer;
	SDL_Window* window;
	UnitManager unitManager;
	int playerNumber;
	std::string playerName;
	std::string SEPERATOR_TOKEN = "!";
	int sendRecvFlag = -1;
	bool quit;
	float delayTimer = 0;


	TCPSocketPtr sendSocket, recvSocket, recvConnSocket;

	PlayerUser();
	PlayerUser(int _flags, int _pnum, int _xpos);
	~PlayerUser();

	void initTcpClient(std::string sendPort, std::string recvPort);
	void initTcpServer(std::string listenPort);

	std::string packageUnitIntoString(int _id);
	void decodeUnitString(std::string _unitString, bool onlyPrint = false);
	void sendUnitIterator(int _it);
	void sendUnitID(int _id);
	void recieveNewUnit(int _id, int _type, Vector2 _pos, Vector2 _size, Color _col);
	int createRandomUnit();
	int deleteRandomUnit();
	void sendUnitDelete(int _id);
	void decodeDeleteString(std::string _deleteString);
	void closeSockets();
	void shutdown();

};