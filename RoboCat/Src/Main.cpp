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


GameListener* gameListener;// = new GameListener();
PerformanceTracker* pPerformanceTracker;

void runGame()
{
	Game::getInstance()->doLoop();
}

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
			//TODO: Recieve Acknowledgement
			/*
			* if (recieved)
			* set Acknowledged true
			* recieve bytes
			* else
			* set !Acknowledged
			*/
			char buffer[8192];
			int32_t bytesReceived = connSocket->Receive(buffer, 4096);
			//TODO: Send Acknowledgement
			/*
			* if(Acknowledged){
			* send Acknowledged
			* else{
			* send NotAcknowledged
			*/
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

			if (rand() % 10 == 1)
			{
				cout << "gonna ignore this packet :( \n";
			}
			else
			{
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
					cout << "packet acknowledged " << id << "\n";
					cont = false;
				}
				else
				{
					cout << "acknowledging packet " << id << "\n";
					if (std::find(recievedPacks.begin(), recievedPacks.end(), id) != recievedPacks.end()) //acknowledgment was lost, do not repeat instructions
					{
						cout << "acknowledgment was lost, do not repeat instructions" << endl;
						cont = false;
					}
					else
					{
						cout << "adding to recieved packets" << endl;
						recievedPacks.push_back(id);
					}
					//Send Acknowledgement

					cout << "sending acknowledgement" << endl;
					connSocket->Send(to_string(id).c_str(), to_string(id).length());
				}

				if (cont)
				{
					//Game::getInstance()->deleteAllUnits();
					size_t pos = 0;
					size_t pos2 = 0;
					std::string token;
					cout << endl;

					size_t tempPos = receivedMsg.find(del3);
					string tempMsg = receivedMsg.substr(0, tempPos);
					receivedMsg.erase(0, tempPos + del3.length());
					cout << tempMsg << " END\n" << receivedMsg << endl;
					while ((pos = tempMsg.find(del1)) != std::string::npos) {
						token = tempMsg.substr(0, pos);
						//cout << "getting a new unit " << token << endl;
						vector<int> unitData;
						while ((pos2 = token.find(del2)) != std::string::npos) {
							//cout << "getting new data from unit" << endl;
							string token2 = token.substr(0, pos);
							unitData.push_back(stoi(token2));
							//cout << stoi(token2) << " ";
							token.erase(0, pos2 + del2.length());
						}
						//cout <<"\n";
						data.push_back(unitData);
						tempMsg.erase(0, pos + del1.length());
					}

					while ((pos2 = receivedMsg.find(del1)) != std::string::npos) {
						//cout << "getting new data from unit" << endl;
						string token2 = receivedMsg.substr(0, pos);
						deletedUnits.push_back(stoi(token2));
						//cout << stoi(token2) << " ";
						receivedMsg.erase(0, pos2 + del1.length());
					}

					//cout << "got all the data " << data.size() << endl;

					for (int i = 0; i < data.size(); i++)
					{
						//cout << "placing unit" << endl << endl;
						if (Game::getInstance()->unitWithID(data[i][0]))
						{
							Game::getInstance()->updateUnitLocation(data[i][0], data[i][2], data[i][3]);
						}
						else
						{
							Game::getInstance()->placeUnit(data[i][0], data[i][1], data[i][2], data[i][3]);
						}
					}

					for (int i = 0; i < deletedUnits.size(); i++)
					{
						Game::getInstance()->deleteUnit(deletedUnits[i]);
					}

					//LOG("%s", receivedMsg.c_str());
					std::cout << ">";
				}
			}
		}
		});
	while (!quit) // Need to add a quit here to have it really exit!
	{
		runGame();

		vector<int> toDelte;
		for (auto& it : unackPacks) {
			if (!it.second.second)
			{
				cout << "resending lost packet " << it.first <<"\n";
				connSocket->Send(it.second.first.c_str(), it.second.first.length());
			}
			else if (it.second.second)
			{
				cout << "deleting acknowledged packet " << it.first <<  "\n";
				toDelte.push_back(it.first);
				//unackPacks.erase(it.first);
			}
		}

		for (int i = 0; i < toDelte.size(); i++)
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

			Game::getInstance()->setWorldStateChanged(false);
			vector<vector<int>> data = Game::getInstance()->getUnitData();
			for (int i = 1; i < data.size(); i++)
			{
				msg += to_string(data[i][0]) + " " + to_string(data[i][1]) + " " + to_string(data[i][2]) + " " + to_string(data[i][3]) + " \n";
			}
			msg += "END";
			vector<int> data2 = Game::getInstance()->getUnitDeletion();
			cout << data2.size() << endl;
			for (int i = 0; i < data2.size(); i++)
			{
				msg += to_string(data2[i]) + " \n";
				cout << data2[i] << endl;
			}

			unackPacks.emplace(id, std::make_pair(msg, false));
			if (rand() % 10 != 5)
			{
				connSocket->Send(msg.c_str(), msg.length());
			}
			cout << "MINE\n" << id << endl;
		}
		//std::this_thread::sleep_for(std::chrono::seconds(1)); //SECONDWAIT
	}
	connSocket->~TCPSocket(); // Forcibly close socket (shouldn't call destructors like this -- make a new function for it!
	receiveThread.join();
}

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
	bool quit = false;
	std::thread receiveThread([&clientSocket, &quit, &unackPacks, &recievedPacks]() {
		while (!quit)
		{
			//TODO: Recieve Acknowledgement
			/*
			* if (recieved)
			* set Acknowledged true
			* recieve bytes
			* else
			* set !Acknowledged
			*/
			char buffer[8192];
			int32_t bytesReceived = clientSocket->Receive(buffer, 4096);

			//TODO: Send Acknowledgement
			/*
			* if(Acknowledged){
			* send Acknowledged
			* else{
			* send NotAcknowledged
			*/
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

			if (rand() % 10 == 0)
			{
				cout << "ignoring this packet :(\n";
			}
			else
			{
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
					cout << "setting packet as acknowledged " << id << endl;
					//Check if acknowledged
					unackPacks.find(id)->second.second = true;
					cont = false;
				}
				else
				{
					cout << "sending packet acknowledgment " << id << endl;
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

				if (cont)
				{
					//Game::getInstance()->deleteAllUnits();
					size_t pos = 0;
					size_t pos2 = 0;
					std::string token;
					cout << endl;

					size_t tempPos = receivedMsg.find(del3);
					string tempMsg = receivedMsg.substr(0, tempPos);
					receivedMsg.erase(0, tempPos + del3.length());
					while ((pos = tempMsg.find(del1)) != std::string::npos) {
						token = tempMsg.substr(0, pos);
						cout << "getting a new unit " << token << endl;
						vector<int> unitData;
						while ((pos2 = token.find(del2)) != std::string::npos) {
							cout << "getting new data from unit" << endl;
							string token2 = token.substr(0, pos);
							unitData.push_back(stoi(token2));
							cout << stoi(token2) << " ";
							token.erase(0, pos2 + del2.length());
						}
						//cout <<"\n";
						data.push_back(unitData);
						tempMsg.erase(0, pos + del1.length());
					}
					
					cout << "got all the data " << data.size() << endl;

					while ((pos2 = receivedMsg.find(del1)) != std::string::npos) {
						string token2 = receivedMsg.substr(0, pos);
						cout << "getting deleted data" << endl;
						deletedUnits.push_back(stoi(token2));
						//cout << stoi(token2) << " ";
						receivedMsg.erase(0, pos2 + del1.length());
					}

					cout << "got all the data for realizies " << data.size() << " " << deletedUnits.size() << endl;

					for (int i = 0; i < data.size(); i++)
					{
						//cout << "placing unit" << endl;
						if (Game::getInstance()->unitWithID(data[i][0]))
						{
							Game::getInstance()->updateUnitLocation(data[i][0], data[i][2], data[i][3]);
						}
						else
						{
							Game::getInstance()->placeUnit(data[i][0], data[i][1], data[i][2], data[i][3]);
						}
					}
					cout << "units placed" << endl;
					for (int i = 0; i < deletedUnits.size(); i++)
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
	while (!quit)
	{
		runGame();

		vector<int> toDelte;
		for (auto& it : unackPacks) {
			// Do stuff
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

		for (int i = 0; i < toDelte.size(); i++)
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
			for (int i = 1; i < data.size(); i++)
			{
				msg += to_string(data[i][0]) + " " + to_string(data[i][1]) + " " + to_string(data[i][2]) + " " + to_string(data[i][3]) + " \n";
			}

			msg += "END";
			vector<int> data2 = Game::getInstance()->getUnitDeletion();
			for (int i = 1; i < data2.size(); i++)
			{
				msg += to_string(data2[i]) + " \n";
			}

			unackPacks.emplace(id, std::make_pair(msg, false));
			if (rand() % 10 != 2)
			{
				clientSocket->Send(msg.c_str(), msg.length());
			}
			cout << "MINE\n" << id << endl;
		}
		/*
		std::cout << ">";
		std::string msg = "Test";
		//std::getline(std::cin, msg);
		if (msg == "/quit")
		{
			quit = true;
		}
		//Game::getInstance()->getUnitData();
		//clientSocket->Send(msg.c_str(), msg.length());
		*/
		//std::this_thread::sleep_for(std::chrono::seconds(1));//SECONDWAIT
	}
	clientSocket->~TCPSocket();
	receiveThread.join();
}

std::mutex coutMutex;

void DoCout(std::string msg)
{
	for (int i = 0; i < 5; i++)
	{
		coutMutex.lock();  // can block!
		std::cout << msg << std::endl;
		coutMutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	std::cout << "Exiting loop gracefully\n";
}

bool gQuit;

void DoCoutLoop(std::string msg)
{
	while (!gQuit)
	{
		coutMutex.lock();  // can block!
		std::cout << msg << std::endl;
		coutMutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	std::cout << "Exiting loop gracfully\n";
}

void DoCoutLoopLocal(std::string msg, const bool& quit)
{
	while (!quit)
	{
		coutMutex.lock();  // can block!
		std::cout << msg << std::endl;
		coutMutex.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	std::cout << "Exiting loop gracfully\n";
}

void DoCoutAndExit(std::string msg)
{
	std::cout << msg << std::endl;

	std::cout << "Exiting 'loop' gracefully\n";
}

void DoThreadExample()
{
	// Thread Example

	// Ex. 1: Two cout's at once

	//DoCout();
	gQuit = false;
	bool quit = false;

	// Lambdas = anonymous functions = Functions with no name.
	//		max(5, 7) <- two 'anonymous' ints
	//			int five = 5, seven = 7; max(five, seven);
	//
	//	Lambda syntax: [](args) {body} <- a lambda!
	//		[] -> captures (can use variables from outside scope of function

	//  TcpSocketPtr s;
	//	std::thread receiveThread([&s]() {
	//			s->Receive(...);
	//		});
	//
	//  ReceiveOnSocket() {
	//		s->Receive		// Not global! What are we referencing here?
	//	}

	std::thread t1(DoCoutLoopLocal, "Hello from thread 1!", std::ref(quit));
	std::thread t2(DoCoutLoopLocal, "Thread 2 reporting in!", std::ref(quit));
	std::thread t3([&quit](std::string msg)
		{
			while (!quit)
			{
				std::cout << msg << std::endl;

				std::cout << "Exiting 'loop' gracefully\n";
			}
		}, "Thread 3 here!");

	std::cout << "Hello from the main thread!\n";

	std::cout << "Press enter to exit at any time.\n\n";
	std::cin.get();

	gQuit = true;
	quit = true;

	t1.join();
	t2.join();
	t3.join();
}

void shutGame()
{
	Game::getInstance()->cleanUpInstance();

	gameListener->cleanup();
	delete(gameListener);

	EventSystem::getInstance()->cleanupInstance();

	delete pPerformanceTracker;

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

	// WinSock2.h
	//    https://docs.microsoft.com/en-us/windows/win32/api/winsock/


	SocketUtil::StaticInit();

	//DoThreadExample();


	bool isServer = StringUtils::GetCommandLineArg(1) == "server";

	//Init Game
	EventSystem::initInstance();
	cout << "1" << endl;

	const double SLEEP_TIME = 5.0;

	pPerformanceTracker = new PerformanceTracker;

	cout << "2" << endl;
	Game::initInstance();
	cout << "3" << endl;

	//EventSystem* mpESystem = EventSystem::getInstance();
	//mpESystem->addListener(MOUSE_EVENT_B, this);
	//mpESystem->addListener(KEY_EVENT_B, this);

	gameListener = new GameListener();
	gameListener->init();
	cout << "4" << endl;

	Game::getInstance()->setFrameRate(60.0);
	cout << "5" << endl;
	//Game Initted
	if (isServer)
	{
		
		DoTcpServer();
	}
	else
	{
		DoTcpClient(StringUtils::GetCommandLineArg(2));
	}
	cout << "6" << endl;
	shutGame();
	SocketUtil::CleanUp();

	return 0;
}