///
/// This code is based on the code provided by the instructor Scott Barrett, as well
/// as wrapper functions made by Adel Talhouk (the latter having header comments indicating such)
/// unless otherwise stated, the rest of the work was done by Brandon Boras (bb) and Nicholas Perell
/// 

// standard libraries


// libraries provided to us
#include "RoboCatPCH.h"
#include "allegro_wrapper_functions-main/GraphicsLibrary.h"
#include "allegro_wrapper_functions-main/InputSystem.h"
#include "GameFiles/Lock.h"
#include "GameFiles/Key.h"
#include "GameFiles/WorldState.h"
#include "GameFiles/NetworkManager.h"
#include <windows.h>

///	Assignment 4
///			TODO
///	X - client-side simulation
/// X - dead reckoning
///	X - Simulate high (250ms+) latency to make this effect very visible. 
///		X - Send a packet from the server to the client no more than twice per second (once every 500ms) to simulate a low-bandwidth environment to make the effect clear.
///	X - Client Prediction Behavior
///		X - The client interpolates state between its own, stale state and the authoritative state it received from the server.
///		X - For determinist objects, it uses the same simulation as the server.
///		X - For non - deterministic objects, it estimates using dead reckoning.
/// X - Client Prediction Implementation
///		O - The server sends messages infrequently
///		O - there is a high amount of latency simulated.
///		X - The client uses the same algorithms as the server where possible, and uses dead reckoning otherwise.
///		X - Both forms of client - side prediction are clearly present.
///	X - There are at least 3 distinct game object types with different data requirements. 
///		O - At least one object can be simulated on the client
///		O - at least one object requires dead reckoning (i.e., is another player)

const float SCREEN_X = 1500;
const float SCREEN_Y = 750;
const string FILE_PATH = "\\..\\..\\images\\";
const string CREATOR_BACKGROUND = "background.jpg";
const string JOINER_BACKGROUND = "background3.jpg";
const int CREATOR_PORT = 8080;
const int JOINER_PORT = 8100;

std::atomic<bool> connectionsOpen(true);

#if _WIN32
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
	
	srand(0);
	rand(); // discarding the first rand() call, which is usually not very random (even for rand()'s standards)

	string userAnswer;
	
	// connection-related

	bool userIsCreator;
	int listeningPort;
	std::thread listeningThread;
	UDPSocketPtr listeningSocket;
	SocketAddressPtr listeningAddress;

	UDPSocketPtr sendingSocket;

	SocketAddressPtr addressToSendTo;
	SocketAddressPtr addressRecievedFrom;

	priority_queue<pair<int, void*>> unprocessedData = priority_queue<pair<int, void*>>();

	string currentBackground;

	//--------------------- intialization window -------------------------------

	SocketUtil::StaticInit();

	// instructions

	std::cout <<
		"Hello, welcome to this game demonstration, here are the controls\n" <<
		"The creator(c) of the game and the joiner(j) have different moves available\n" <<
		"Left Click (c) - Creates a lock that moves from the left to the right.\n" <<
		"Left Click (j) - Creates a key that moves from the right to the left.\n\n" <<
		"If a lock and key touch then they spawn a coin.\n\n" <<
		
		"Another thing to note is that any errors in connection will be displayed in the command line.\n\n";

	// pre-game menu
	std::cout << "\n\nWould you like to create (type c) or join (type j) a match of this game?\n";
	do
	{
		std::cin >> userAnswer;
		std::cin.clear();
	} while (!(userAnswer == "c" || userAnswer == "C" || userAnswer == "j" || userAnswer == "J"));

	userIsCreator = (userAnswer == "c" || userAnswer == "C");

	
	// handling the initial networking set up
	if (userIsCreator)
	{
		std::cout << "Thank you for your answer, we will now wait for someone to join the game.\n";


		listeningPort = CREATOR_PORT;
		NetworkManager::SetUpInitialListening(listeningPort, listeningSocket, listeningAddress);
		
		addressRecievedFrom = SocketAddressFactory::CreateIPv4FromString((NetworkManager::ACCEPT_ALL_ADDRESS + std::to_string(CREATOR_PORT+5)));
		//HandleListening(closingConnections, listeningSocket, addressToSendTo, unprocessedData, waitingForConnection);
		//something about this thread is cursed, throws errors in the actual implementation of thread
		listeningThread = std::thread(NetworkManager::HandleListening, &(connectionsOpen), listeningSocket, addressRecievedFrom, ref(unprocessedData));

		do
		{
			Sleep(500); // wait 0.5 second
			std::cout << '.';
		} while (unprocessedData.size() < 1);

		std::cout << "\n Recieved an offer to start the game, now sending the reply to start their game.\n";
		NetworkManager::SetUpSending(JOINER_PORT, CREATOR_PORT + 10, sendingSocket, addressToSendTo);
	}
	else // try to connect to a host
	{
		
		NetworkManager::SetUpSending(CREATOR_PORT, JOINER_PORT + 10, sendingSocket, addressToSendTo);
		std::cout << "\nSent the request to join, waiting for a reply.\n";
		
		bool waitingForConnection = true;

		listeningPort = JOINER_PORT;
		NetworkManager::SetUpInitialListening(listeningPort, listeningSocket, listeningAddress);

		addressRecievedFrom = SocketAddressFactory::CreateIPv4FromString((NetworkManager::ACCEPT_ALL_ADDRESS + std::to_string( JOINER_PORT + 5)));
		listeningThread = std::thread(NetworkManager::HandleListening, &(connectionsOpen), listeningSocket, addressRecievedFrom, ref(unprocessedData));
		do
		{
			Sleep(500); // wait 0.5 second
			std::cout << '.';
		} while (unprocessedData.size() < 1);
	}

	std::cout << "\n\nStarting the game!\n";

	// clearing the queue
	unprocessedData = priority_queue<pair<int, void*>>();

	//  Graphics init
	GraphicsLibrary gl(SCREEN_X, SCREEN_Y);
	if ( !gl.init())
		std::cout << "ERROR INTIALIZING THE GRAPHICS LIBRARY\n";

	// Input Init
	InputSystem inputSys;
	if (!inputSys.init(&(gl)))
		std::cout << "ERROR INITIALIZING THE INPUT SYSTEM\n";

	// load background

	currentBackground = userIsCreator ? CREATOR_BACKGROUND : JOINER_BACKGROUND;
	gl.loadImage(FILE_PATH, currentBackground);
	gl.drawImage(currentBackground, 0, 0);


	gl.render();

	gl.loadImage(FILE_PATH, "lock_blue.png");
	gl.loadImage(FILE_PATH, "keyBlue.png");
	gl.loadImage(FILE_PATH, "coinGold.png");
	gl.loadImage(FILE_PATH, "hud_p1.png");

	WorldState gameWorld = WorldState(&gl);

	if (userIsCreator)
	{
		//gameWorld.CreateLock();
		//gameWorld.Create<Key>(SCREEN_X,0);
	}

	gameWorld.CreatePlayers();

	vector<JoinerInput> joinerInputs;


	system_clock::time_point lastTime = system_clock::now();
	system_clock::time_point startTime = system_clock::now();

	system_clock::time_point lastTimeOfRecievingConnection = system_clock::now();
	system_clock::time_point lastTimeOfSendingConnection = system_clock::now();

	//"Send a packet from the server to the client no more than twice per second 
	//(once every 500ms) to simulate a low-bandwidth environment to make the effect clear."
	int latencyTimer = 0;
	const int LATENCY_TIME = 500;

	int deltaTime;
	const int TIME_STEP = 16;
	bool isGameRunning = true;

	// `````````````````````````  main game loop  ``````````````````````````` 
	while (isGameRunning)
	{
		deltaTime = duration_cast<milliseconds>(system_clock::now() - lastTime).count();
		startTime = lastTime;
		lastTime = system_clock::now();
		latencyTimer += deltaTime;

		//std::cout << 0 << '\n';

		inputSys.Update(userIsCreator, ref(gameWorld), ref(joinerInputs));
		//std::cout << 1 << '\n';
		if (userIsCreator)
		{
			//std::cout << 2 << '\n';
			// processing Joiner's inputs before update means that their spawned pieces get exported in the world state
			if(!NetworkManager::HandleIncomingInputPackets(ref(unprocessedData), ref(joinerInputs), ref(lastTimeOfRecievingConnection)))
			{
				isGameRunning = false;
			}
			//std::cout << 3 << '\n';
			gameWorld.Update(userIsCreator, ref(joinerInputs), deltaTime);

			//std::cout << 4 << '\n';
			if (latencyTimer > LATENCY_TIME)
			{
				NetworkManager::HandleOutgoingWorldStatePackets(ref(gameWorld), sendingSocket, addressToSendTo, deltaTime);
				latencyTimer = 0;
			}
		}
		else
		{
			//std::cout << 2 << '\n';
			gameWorld.Update(userIsCreator, ref(joinerInputs), deltaTime);
			//ProcessWorldState();
			//std::cout << 3 << '\n';
			if (!NetworkManager::HandleIncomingWorldStatePackets(ref(gameWorld), ref(unprocessedData), ref(lastTimeOfRecievingConnection)))
			{
				isGameRunning = false;
			}
			NetworkManager::HandleOutgoingInputPackets(ref(joinerInputs), sendingSocket, addressToSendTo, ref(lastTimeOfSendingConnection), deltaTime);
			latencyTimer = 0;
			//std::cout << 4 << '\n';
		}

		//std::cout << 5 << '\n';
		gameWorld.Render(currentBackground);
		//std::cout << 6 << '\n';


		while (duration_cast<milliseconds>(system_clock::now() - startTime).count() < TIME_STEP)
		{
			;
			// makes the program maintain a maximum framerate
			//std::cout << duration_cast<milliseconds>(system_clock::now() - startTime).count() << std::endl;
		}
	}


	// ______________________________   Clean Up ______________________________

	gl.CleanUp();
	connectionsOpen = false;

	listeningThread.join();
	SocketUtil::CleanUp();

	return 0;
}

