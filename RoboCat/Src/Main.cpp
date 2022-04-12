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

///          TODO
///  O - Handle one player disconnecting on the other player's end
///		O - Joiner doesn't crash, should DC when worldstate takes too long to update
///		O - Creator doesn't crash, joiner should send a "all-ok" message if the Joiner hasn't pressed an input in a while
///  O - Update gameobjects via a frame delta time (to prevent desync due to game speed)
///  X - Unreliability Simulation -
///     O - Packets are occasionally "dropped," by random chance or by some algorithm.
///     O - Dropped packets are registered as having been sent by the sender, but do not actually call the socket's send() function. 
///     X - Random delay is introduced to some packet's send time, (technically in, but a bit of a reach) 
///		X - and packets can be delivered out of order.
///  X - Reliability - 
///     X - Game state is sent through a reliability layer. Important data is sent through guaranteed mechanisms, 
///            while data that does not require guaranteed delivery is sent through simpler mechanisms.
///     X - Important data is resilient to jitter, out - of - order delivery, and high latency.
///  X - Game Complexity - 
///     X - There are at least 3 distinct game object types with different data requirements.
///     X - Some game objects require complex types, such as collections or pointers, which are properly replicated.
///  X - Code Cleanliness - 
///            Spacing is consistent, variable naming is consistent.
///            Often - repeated code is organized into functions or classes, as appropriate.
///            Variables are appropriately scoped(i.e., they don't outlive their usefulness). 
///            Variable scope is kept manageable by migrating code to functions when appropriate. 
///            Related code is kept in the same place. Unused or commented code is not present.
///  X - Code Sophistication - 
///         Code is robust in dealing with errors and malformed input. Preprocessor macros, gotos, global variables, 
///            and similar "code smell" constructs are not used without strong justification. No resources are leaked,
///            including but not limited to memory, files, sockets, and mutexes. Member functions and their parameters
///            are declared const whenever appropriate. No warnings are generated by the compiler.
///
///       Last touches
///  x - switch srand around line 81 of main from beinbg seeded by 0 to being seeded by current time (it was 0 for replicatability) 
///  X - maybe user inputs should also be sent over a priority queue?
///  X - update the control prompt at the beginning of the game
///  X - when user wants to quit, set  closingConnections = true; (to clean things up)
///  X - make sure there is no git trash left over like .orig
///  X - clean up every cout in the program for final
///  X - all compiler errors



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
		"Left Clock (j) - Creates a key that moves from the right to the left.\n\n" <<
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

	WorldState gameWorld = WorldState(&gl);

	if (userIsCreator)
	{
		//gameWorld.CreateLock();
		//gameWorld.Create<Key>(SCREEN_X,0);
	}

	vector<JoinerInput> joinerInputs;


	system_clock::time_point lastTime = system_clock::now();
	system_clock::time_point startTime = system_clock::now();

	system_clock::time_point lastTimeOfRecievingConnection = system_clock::now();
	system_clock::time_point lastTimeOfSendingConnection = system_clock::now();


	int deltaTime;
	int timestep = 16;
	bool isGameRunning = true;

	// `````````````````````````  main game loop  ``````````````````````````` 
	while (isGameRunning)
	{
		deltaTime = duration_cast<milliseconds>(system_clock::now() - lastTime).count();
		startTime = lastTime;
		lastTime = system_clock::now();

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
			NetworkManager::HandleOutgoingWorldStatePackets(ref(gameWorld), sendingSocket, addressToSendTo, deltaTime);
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
			//std::cout << 4 << '\n';
		}

		//std::cout << 5 << '\n';
		gameWorld.Render(currentBackground);
		//std::cout << 6 << '\n';


		while (duration_cast<milliseconds>(system_clock::now() - startTime).count() < 16)
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

