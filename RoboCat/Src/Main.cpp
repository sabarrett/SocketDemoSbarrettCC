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
#include "GameFiles/WorldState.h"
#include "GameFiles/NetworkManager.h"


///
///          TODO
///  X - Synchronize game objects over a socket.
///  X - Create a two-player networked game with shared world state.
///  X - Use Memory Streams and different Packet Header types to synchronize the state between the two instances.
///  X - At least 10 synchronized objects in the world at once.
///  X - At least 3 different kinds of game objects.
///  X - Involve creating and destroying objects, which is then synchronized between clients.
///  O - Use TCP and UDP appropriately, (for deltas and world state respectively) 
///    X - if using UDP, there is a chance of things arriving out of order, so note the time of last remote update and send the time of the update with it, discarding old ones
///  X - when user wants to quit, set  closingConnections = true; (to clean things up)



const float SCREEN_X = 1500;
const float SCREEN_Y = 750;
const string FILE_PATH = "\\..\\..\\images\\";
const string BACKGROUND = "background.jpg";
const int CREATOR_PORT = 8080;
const int JOINER_PORT = 8100;

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
	
	
	string userAnswer;
	
	// connection-related
	bool userIsCreator;
	bool connectionsOpen = true;
	int listeningPort;
	std::thread listeningThread;
	UDPSocketPtr listeningSocket;
	SocketAddressPtr listeningAddress;

	UDPSocketPtr sendingSocket;

	SocketAddressPtr addressToSendTo;
	SocketAddressPtr addressRecievedFrom;

	vector<std::pair<int, void*>> unprocessedData = vector<std::pair<int, void*>>();

	//--------------------- intialization window -------------------------------

	SocketUtil::StaticInit();


	// instructions

	std::cout <<
		"Hello, welcome to this game demonstration, here are the controls\n" <<
		"The creator(c) of the game and the joiner(j) have different moves available" <<
		"Left Click (c) - Creates a lock at that position";

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
		listeningThread = std::thread(NetworkManager:: HandleListening, std::ref(connectionsOpen), listeningSocket, addressRecievedFrom, std::ref(unprocessedData));

		do
		{
			Sleep(500); // wait 0.5 second
			std::cout << '.';
		} while (unprocessedData.size() < 1);

		std::cout << "\n Recieved an offer to start the game, now sending the reply to start their game.\n";
		NetworkManager::SetUpSending(JOINER_PORT, CREATOR_PORT + 10, sendingSocket, addressToSendTo);


		std::cout << "\n\nStarting the game!\n";


		// set up sending to joiner here

	}
	else // try to connect to a host
	{
		
		NetworkManager::SetUpSending(CREATOR_PORT, JOINER_PORT + 10, sendingSocket, addressToSendTo);
		std::cout << "\nSent the request to join, waiting for a reply.\n";
		
		bool waitingForConnection = true;

		//int listeningPort = JOINER_PORT;

		listeningPort = JOINER_PORT;
		NetworkManager::SetUpInitialListening(listeningPort, listeningSocket, listeningAddress);

		addressRecievedFrom = SocketAddressFactory::CreateIPv4FromString((NetworkManager::ACCEPT_ALL_ADDRESS + std::to_string( JOINER_PORT + 5)));
		listeningThread = std::thread(NetworkManager::HandleListening, std::ref(connectionsOpen), listeningSocket, addressRecievedFrom, std::ref(unprocessedData));
		do
		{
			Sleep(500); // wait 0.5 second
			std::cout << '.';
		} while (unprocessedData.size() < 1);
	}

	// we don't need the starting messages stored
	unprocessedData.clear();

	//  Graphics init
	GraphicsLibrary gl(SCREEN_X, SCREEN_Y);
	if ( !gl.init())
		std::cout << "ERROR INTIALIZING THE GRAPHICS LIBRARY\n";

	// Input Init
	InputSystem inputSys;
	if (!inputSys.init(&(gl)))
		std::cout << "ERROR INITIALIZING THE INPUT SYSTEM\n";

	// load background
	gl.loadImage(FILE_PATH, BACKGROUND);
	gl.drawImage(BACKGROUND, 0, 0);
	gl.render();

	gl.loadImage(FILE_PATH, "lock_blue.png");

	WorldState gameWorld = WorldState(&gl);
	gameWorld.CreateLock();

	vector<JoinerInput> joinerInputs;


	// `````````````````````````  main game loop  ``````````````````````````` 
	while (true)
	{
		inputSys.Update(userIsCreator, std::ref(gameWorld), std::ref(joinerInputs));

		if (userIsCreator)
		{
			NetworkManager::HandleIncomingInputPackets(); // this way player 2's inputs don't just get squashed by player 1's world state being definitive
			gameWorld.Update();
			//ProcessWorldState();

			NetworkManager::HandleOutgoingWorldStatePackets();
		}
		else
		{
			gameWorld.Update();
			//ProcessWorldState();
			NetworkManager::HandleIncomingWorldStatePackets();
			NetworkManager::HandleOutgoingInputPackets(std::ref(joinerInputs));
		}


		//  Render

		gameWorld.Render();

		;
	}


	// ______________________________   Clean Up ______________________________

	connectionsOpen = false;

	listeningThread.join();
	SocketUtil::CleanUp();

	return 0;
}

