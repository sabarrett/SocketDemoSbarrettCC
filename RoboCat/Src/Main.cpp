#include "RoboCatPCH.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <iostream>
#include <cassert>
#include <string>
#include <sstream>

//Game Includes
#include <PerformanceTracker.h>
#include <MemoryTracker.h>
#include <Game.h>
#include "allegro5/allegro.h"
#include <EventSystem.h>
#include <GameEventSystem.h>
#include <GameListener.h>

/// <summary>
/// Run the game
/// </summary>
void runGame()
{
	Game::getInstance()->doLoop();
}

/// <summary>
/// Create TCP Server Program to send and recieve data
/// </summary>
void DoTcpServer()
{
	// Create socket
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}
	unordered_map<int, pair<string, bool>> unackPacks;
	vector<int> recievedPacks;


	//listenSocket->SetNonBlockingMode(true);

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

	// Blocking function call -> Waits for some input; halts the program until something "interesting" happens
	// Non-Blocking function call -> Returns right away, as soon as the action is completed

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
		// SocketUtil::ReportError("Accepting connection");
		// ExitProcess(1);
	}

	LOG("Accepted connection from %s", incomingAddress.ToString().c_str());

	bool quit = false;
	std::thread receiveThread([&connSocket, &quit, &incomingAddress, &unackPacks, &recievedPacks]() { // don't use [&] :)
		while (!quit) // Need to add a quit here to have it really exit!
		{
			//Create Buffer
			char buffer[8192];
			int32_t bytesReceived = connSocket->Receive(buffer, 4096);
			//Check recieved buffer for any data
			if (bytesReceived == 0)
			{
				std::cout << incomingAddress.ToString() << " has disconnected";
				quit = true;
			}
			if (bytesReceived < 0)
			{
				SocketUtil::ReportError("Receiving");
				return;
			}

			//Random Ignore Packet
			if (rand() % 10 == 1)
			{
				std::cout << "gonna ignore this packet :( \n";
			}
			else
			{
				//Recieve data
				std::string receivedMsg(buffer, bytesReceived);
				vector<vector<int>> data;
				vector<int> deletedUnits;

				string del1 = "\n";
				string del2 = " ";
				string del3 = "END";

				//Get Package ID
				size_t idPos = 0;
				idPos = receivedMsg.find(del1);
				int id = stoi(receivedMsg.substr(0, idPos));
				receivedMsg.erase(0, idPos + del1.length());

				bool cont = true;

				if (unackPacks.find(id) != unackPacks.end())
				{
					//Check if acknowledged
					unackPacks.find(id)->second.second = true;
					cont = false;
				}
				else
				{
					std::cout << "acknowledging packet " << id << "\n";
					if (std::find(recievedPacks.begin(), recievedPacks.end(), id) != recievedPacks.end()) 
					{
						//acknowledgment was lost, do not repeat instructions
						cont = false;
					}
					else
					{
						//Acknowledgement was found push packet to be interpreted
						recievedPacks.push_back(id);
					}


					//Send Acknowledgement
					connSocket->Send(to_string(id).c_str(), to_string(id).length());
				}

				//If packets were aquired load the data into the game
				if (cont)
				{
					//Game::getInstance()->deleteAllUnits();
					size_t pos = 0;
					size_t pos2 = 0;
					std::string token;
					std::cout << endl;

					size_t tempPos = receivedMsg.find(del3);
					string tempMsg = receivedMsg.substr(0, tempPos);
					receivedMsg.erase(0, tempPos + del3.length());
					std::cout << tempMsg << " END\n" << receivedMsg << endl;
					while ((pos = tempMsg.find(del1)) != std::string::npos) {
						token = tempMsg.substr(0, pos);
						//Add new Unit
						vector<int> unitData;
						while ((pos2 = token.find(del2)) != std::string::npos) {
							//Getting new Unit position
							string token2 = token.substr(0, pos);
							unitData.push_back(stoi(token2));
							token.erase(0, pos2 + del2.length());
						}
						//Add Unit to the data to get updated
						data.push_back(unitData);
						tempMsg.erase(0, pos + del1.length());
					}

					while ((pos2 = receivedMsg.find(del1)) != std::string::npos) {
						//Get new Unit data
						string token2 = receivedMsg.substr(0, pos);
						deletedUnits.push_back(stoi(token2));
						receivedMsg.erase(0, pos2 + del1.length());
					}

					//All data aquired

					for (UINT i = 0; i < data.size(); i++)
					{
						//Update Unit position
						if (Game::getInstance()->unitWithID(data[i][0]))
						{
							Game::getInstance()->updateUnitLocation(data[i][0], data[i][2], data[i][3]);
						}
						else
						{
							//Place new Units
							Game::getInstance()->placeUnit(data[i][0], data[i][1], data[i][2], data[i][3]);
						}
					}

					for (UINT i = 0; i < deletedUnits.size(); i++)
					{
						//Delete deleted units
						Game::getInstance()->deleteUnit(deletedUnits[i]);
					}
				}
			}
		}
		});
	while (!quit)
	{
		runGame();

		vector<int> toDelte;
		for (auto& it : unackPacks) {
			if (!it.second.second)
			{
				//If data lost resend the data
				std::cout << "resending lost packet " << it.first <<"\n";
				connSocket->Send(it.second.first.c_str(), it.second.first.length());
			}
			else if (it.second.second)
			{
				//Delete packets
				std::cout << "deleting acknowledged packet " << it.first <<  "\n";
				toDelte.push_back(it.first);
			}
		}

		for (UINT i = 0; i < toDelte.size(); i++)
		{
			unackPacks.erase(toDelte[i]);
		}

		if (Game::getInstance()->getWorldStateChanged())
		{
			int id = (rand() % 2000) + 2001;

			while (unackPacks.find(id) != unackPacks.end())
			{
				int id = (rand() % 2000) + 2001;
			}

			string msg = to_string(id) + "\n";
			//Begin transferring data
			Game::getInstance()->setWorldStateChanged(false);
			vector<vector<int>> data = Game::getInstance()->getUnitData();
			for (UINT i = 1; i < data.size(); i++)
			{
				msg += to_string(data[i][0]) + " " + to_string(data[i][1]) + " " + to_string(data[i][2]) + " " + to_string(data[i][3]) + " \n";
			}
			msg += "END";
			//Add unit deletion
			vector<int> data2 = Game::getInstance()->getUnitDeletion();
			for (UINT i = 0; i < data2.size(); i++)
			{
				msg += to_string(data2[i]) + " \n";
			}
			//Send data
			unackPacks.emplace(id, std::make_pair(msg, false));
			if (rand() % 10 != 5)
			{
				connSocket->Send(msg.c_str(), msg.length());
			}
		}
	}
	connSocket->~TCPSocket(); // Forcibly close socket (shouldn't call destructors like this -- make a new function for it!
	receiveThread.join();
}

/// <summary>
/// Create TCP Client Program to send and recieve data
/// </summary>
/// <param name="port">Client port to connect to the server</param>
void DoTcpClient(std::string port)
{
	// Create socket
	unordered_map<int, pair<string,bool>> unackPacks;
	vector<int> recievedPacks;
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
	}

	LOG("%s", "Client socket created");

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
		ExitProcess(1);
	}

	LOG("%s", "Bound client socket");
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
	bool quit = false;

	//Recieve game data from server
	std::thread receiveThread([&clientSocket, &quit, &unackPacks, &recievedPacks]() {
		while (!quit)
		{
			//Create Buffer
			char buffer[8192];
			int32_t bytesReceived = clientSocket->Receive(buffer, 4096);
			
			//Check how many bytes have been recieved
			if (bytesReceived == 0)
			{
				std::cout << "Server disconnected";
				quit = true;
			}
			if (bytesReceived < 0)
			{
				SocketUtil::ReportError("Receiving");
				return;
			}

			//Variation of randomness
			if (rand() % 10 == 0)
			{
				cout << "ignoring this packet :(\n";
			}
			//Recieve packets
			else
			{
				//Create recieved message to send back
				std::string receivedMsg(buffer, bytesReceived);

				
				vector<vector<int>> data;
				vector<int> deletedUnits;

				string del1 = "\n";
				string del2 = " ";
				string del3 = "END";

				//Get Package ID
				size_t idPos = 0;
				idPos = receivedMsg.find(del1);
				int id = stoi(receivedMsg.substr(0, idPos));
				receivedMsg.erase(0, idPos + del1.length());

				bool cont = true;

				//Check if acknowledged
				if (unackPacks.find(id) != unackPacks.end())
				{
					cout << "setting packet as acknowledged " << id << endl;
					unackPacks.find(id)->second.second = true;
					cont = false;
				}
				else
				{
					//Send Acknowledgement Packet 
					if (std::find(recievedPacks.begin(), recievedPacks.end(), id) != recievedPacks.end()) //acknowledgment was lost, do not repeat instructions
					{
						cont = false;
					}
					else
					{
						recievedPacks.push_back(id);
					}
					//Send Acknowledgement
					clientSocket->Send(to_string(id).c_str(), to_string(id).length());
				}

				//If the packet was aquired then translate the data otherwise disregard
				if (cont)
				{
					
					size_t pos = 0;
					size_t pos2 = 0;
					std::string token;
					cout << endl;
					size_t tempPos = receivedMsg.find(del3);
					string tempMsg = receivedMsg.substr(0, tempPos);
					receivedMsg.erase(0, tempPos + del3.length());
					while ((pos = tempMsg.find(del1)) != std::string::npos) {
						token = tempMsg.substr(0, pos);
						//Getting new Unit
						vector<int> unitData;
						while ((pos2 = token.find(del2)) != std::string::npos) {
							//Getting units data
							string token2 = token.substr(0, pos);
							unitData.push_back(stoi(token2));
							cout << stoi(token2) << " ";
							token.erase(0, pos2 + del2.length());
						}
						//Add this unit to the data list
						data.push_back(unitData);
						tempMsg.erase(0, pos + del1.length());
					}
					
					//Unit Position Data Aquired
					while ((pos2 = receivedMsg.find(del1)) != std::string::npos) {
						string token2 = receivedMsg.substr(0, pos);
						//Check deleted Units
						deletedUnits.push_back(stoi(token2));
						receivedMsg.erase(0, pos2 + del1.length());
					}

					//All Data Aquired

					for (UINT i = 0; i < data.size(); i++)
					{
						//Update Unit location based on Unit ID
						if (Game::getInstance()->unitWithID(data[i][0]))
						{
							Game::getInstance()->updateUnitLocation(data[i][0], data[i][2], data[i][3]);
						}
						//If its a new Unit place it with new ID
						else
						{
							Game::getInstance()->placeUnit(data[i][0], data[i][1], data[i][2], data[i][3]);
						}
					}
					//All Units updated/added

					//Delete units
					for (UINT i = 0; i < deletedUnits.size(); i++)
					{
						if (Game::getInstance()->unitWithID(deletedUnits[i]) != nullptr)
						{
							cout << "deleting unit " << deletedUnits[i] << endl;
							Game::getInstance()->deleteUnit(deletedUnits[i]);
							//cout << "unit deleted " << deletedUnits[i] << endl;
						}
						else
						{
							cout << "there is no unit with id " << deletedUnits[i] << endl;
						}
					}

					//LOG("%s", receivedMsg.c_str());
					std::cout << ">";
				}
			}
		}
		});
	//End of Recieve Thread


	while (!quit)
	{
		runGame();

		vector<int> toDelte;
		for (auto& it : unackPacks) {
			// Resend packets that have been dropped
			if (!it.second.second)
			{
				cout << "resending lost packet " << it.first << endl;
				clientSocket->Send(it.second.first.c_str(), it.second.first.length());
			}
			else if (it.second.second)
			{
				cout << "deleting acknowledged packet " << it.first << endl;
				toDelte.push_back(it.first);
				//unackPacks.erase(it.first);
			}
		}

		for (UINT i = 0; i < toDelte.size(); i++)
		{
			unackPacks.erase(toDelte[i]);
		}

		if (Game::getInstance()->getWorldStateChanged())
		{
			int id = rand() % 2000;

			while (unackPacks.find(id) != unackPacks.end())
			{
				int id = rand() % 2000;
			}

			string msg = to_string(id) + " ";
			Game::getInstance()->setWorldStateChanged(false);
			vector<vector<int>> data = Game::getInstance()->getUnitData();
			//Send the game state
			for (UINT i = 1; i < data.size(); i++)
			{
				msg += to_string(data[i][0]) + " " + to_string(data[i][1]) + " " + to_string(data[i][2]) + " " + to_string(data[i][3]) + " \n";
			}

			//Send unit deletions
			msg += "END";
			vector<int> data2 = Game::getInstance()->getUnitDeletion();
			for (UINT i = 1; i < data2.size(); i++)
			{
				msg += to_string(data2[i]) + " \n";
			}
			//Attempt to send Unit data
			unackPacks.emplace(id, std::make_pair(msg, false));
			if (rand() % 10 != 2)
			{
				clientSocket->Send(msg.c_str(), msg.length());
			}
		}
	}
	clientSocket->~TCPSocket();
	receiveThread.join();
}

/// <summary>
/// End game
/// </summary>
/// <param name="gameListener">Game Event Manager</param>
/// <param name="performanceTracker">Pointer performance tracker</param>
void shutGame(GameListener* gameListener, PerformanceTracker* performanceTracker)
{
	Game::getInstance()->cleanUpInstance();

	gameListener->cleanup();
	delete(gameListener);

	EventSystem::getInstance()->cleanupInstance();

	delete performanceTracker;

	MemoryTracker::getInstance()->reportAllocations(cout);
	system("pause");
}

#if _WIN32
int main(int argc, const char** argv)
{
	cout << "0" << endl;
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

	GameListener* gameListener;// = new GameListener();
	PerformanceTracker* pPerformanceTracker;

	SocketUtil::StaticInit();

	bool isServer = StringUtils::GetCommandLineArg(1) == "server";

	//Init Game
	EventSystem::initInstance();
	//Game Initted

	pPerformanceTracker = new PerformanceTracker;

	//Performance Tracker created
	Game::initInstance();
	//Create insance of game
	gameListener = new GameListener();
	gameListener->init();
	//Create the Game Event Manager

	Game::getInstance()->setFrameRate(60.0);

	//Game Initted
	if (isServer)
	{
		DoTcpServer();
	}
	else
	{
		DoTcpClient(StringUtils::GetCommandLineArg(2));
	}
	//Program success
	shutGame(gameListener, pPerformanceTracker);
	SocketUtil::CleanUp();

	return 0;
}