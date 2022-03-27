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

// can probably be bigger since we're using UDP
const int BUFFER_SIZE = 4096;

const string HOME_ADDRESS = "127.0.0.1:";
const string ACCEPT_ALL_ADDRESS = "0.0.0.0:";

const float SCREEN_X = 1000;
const float SCREEN_Y = 500;
const string FILE_PATH = "\\..\\..\\images\\";
const string BACKGROUND = "background.jpg";
const int CREATOR_PORT = 8080;
const int JOINER_PORT = 8100;


// ~~~~~~~~~~~~~~~~~~~~~   Listening   ~~~~~~~~~~~~~~~~~~~~~~~
void SetUpInitialListening(int& port, UDPSocketPtr& listeningSocket, SocketAddressPtr& listeningAddress)
{
	// here we can select which IPV we're using, and IPV6 is a bit wild for these purposes so, we go with IPV4
	listeningSocket = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	if (listeningSocket == nullptr)
	{
		SocketUtil::ReportError("Error Creating Listen Socket");
		ExitProcess(1);
	}
	//LOG("%s", "created listening socket");

	// lol true sets it to blocking mode, idk why
	/*if(listeningSocket->SetNonBlockingMode(true) != NO_ERROR)   
		SocketUtil::ReportError("Error Setting To Blocking mode");*/

	listeningAddress = SocketAddressFactory::CreateIPv4FromString((ACCEPT_ALL_ADDRESS + std::to_string(port++)));
	if (listeningAddress == nullptr)
	{
		SocketUtil::ReportError("Error creating listening address");
		ExitProcess(1);
	}

	//LOG("%s", "binding the socket");
	while (listeningSocket->Bind(*listeningAddress) != NO_ERROR)
	{
		//LOG("%s", "port: 0.0.0.0:" + std::to_string(nextAvailablePort) + " taken, trying to use port: 0.0.0.0:" + std::to_string(nextAvailablePort + 1));
		listeningAddress = SocketAddressFactory::CreateIPv4FromString(ACCEPT_ALL_ADDRESS + std::to_string(port));
	}

	//LOG("%s", "bound the socket");	
	//if (listeningSocket->ReceiveFrom() != NO_ERROR)
	//{
	//	SocketUtil::ReportError("Error listening with the socket");
	//	ExitProcess(1);
	//}

	LOG("Your port is %i\n", static_cast<int>(port));
	//LOG("%s", "socket is now listening");
}
void HandleListening(bool& connectionsOpen, UDPSocketPtr listeningSocket, SocketAddressPtr addressRecievedFrom, vector<std::pair<int, void*>>& unprocessedData)
{
	std::cout << "Listening Now!"; 
	
	while (connectionsOpen)
	{
		//LOG("%s", "In handleListening before Recieve\n");

		char buffer[BUFFER_SIZE];
		listeningSocket->ReceiveFrom(buffer, BUFFER_SIZE, *(addressRecievedFrom));
	
		//LOG("%s", "In handleListening aft	er Recieve\n");
		

		if (buffer != nullptr)
		{
			string msgRecieved(static_cast<char*>(buffer), BUFFER_SIZE);
			LOG("Recieved message: %s", msgRecieved.c_str());
			unprocessedData.push_back(std::pair<int, void*>(time(0), buffer));
		}
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~ Sending ~~~~~~~~~~~~~~~~~~~~~~~~~~
void SetUpSending(int portToSendTo, int portUsedForSending, UDPSocketPtr sendingSocket, SocketAddressPtr sendingAddress)
{
	UDPSocketPtr newSendingSocket = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	if (newSendingSocket == nullptr)
	{
		SocketUtil::ReportError("Error Creating Sending Socket");
		ExitProcess(1);
	}
	//LOG("%s", "created connection socket");


	// make sure you send to THIS address, not the folloing one
	SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString((ACCEPT_ALL_ADDRESS + std::to_string(portUsedForSending)));
	if (a == nullptr)
	{
		SocketUtil::ReportError("Error creating sending address");
		ExitProcess(1);
	}
	//LOG("%s", "created connection socket address");

	//LOG("%s", "binding the connection socket");
	while (newSendingSocket->Bind(*a) != NO_ERROR)
	{
		a = SocketAddressFactory::CreateIPv4FromString((HOME_ADDRESS + std::to_string(++portUsedForSending)));
	}
	//LOG("%s", "finished binding the connection socket");


	//LOG("%s%i", "Sending message to 127.0.0.1:", portToSendTo);
    sendingAddress = SocketAddressFactory::CreateIPv4FromString((HOME_ADDRESS + std::to_string(portToSendTo)));  // this has to match the server's address, and it MUST NOT match client's own
	if (sendingAddress == nullptr)
	{
		SocketUtil::ReportError("Creating foreign listener address");
		ExitProcess(1);
	}

	string msg("Let's start a game?");
	if ((newSendingSocket->SendTo(msg.c_str(), BUFFER_SIZE, *sendingAddress)) < 0)
	{
		SocketUtil::ReportError("Sending First Message");
	}

	LOG("%s\n", "Starting message sent.");
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

	
	if (userIsCreator)
	{
		std::cout << "Thank you for your answer, we will now wait for someone to join the game.\n";


		listeningPort = CREATOR_PORT;
		SetUpInitialListening(listeningPort, listeningSocket, listeningAddress);
		
		addressRecievedFrom = SocketAddressFactory::CreateIPv4FromString((ACCEPT_ALL_ADDRESS + std::to_string(CREATOR_PORT+5)));
		//HandleListening(closingConnections, listeningSocket, addressToSendTo, unprocessedData, waitingForConnection);
		//something about this thread is cursed, throws errors in the actual implementation of thread
		listeningThread = std::thread(HandleListening, std::ref(connectionsOpen), listeningSocket, addressRecievedFrom, std::ref(unprocessedData));

		do
		{
			Sleep(500); // wait 0.5 second
			std::cout << '.';
		} while (unprocessedData.size() < 1);

		std::cout << "\n Recieved an offer to start the game, now sending the reply to start their game.\n";
		SetUpSending(JOINER_PORT, CREATOR_PORT + 10, sendingSocket, addressToSendTo);


		std::cout << "\n\nStarting the game!\n";


		// set up sending to joiner here

	}
	else // try to connect to a host
	{
		
		SetUpSending(CREATOR_PORT, JOINER_PORT + 10, sendingSocket, addressToSendTo);
		std::cout << "\nSent the request to join, waiting for a reply.\n";
		
		bool waitingForConnection = true;

		//int listeningPort = JOINER_PORT;

		listeningPort = JOINER_PORT;
		SetUpInitialListening(listeningPort, listeningSocket, listeningAddress);

		addressRecievedFrom = SocketAddressFactory::CreateIPv4FromString((ACCEPT_ALL_ADDRESS + std::to_string( JOINER_PORT + 5)));
		listeningThread = std::thread(HandleListening, std::ref(connectionsOpen), listeningSocket, addressRecievedFrom, std::ref(unprocessedData));
		do
		{
			Sleep(500); // wait 0.5 second
			std::cout << '.';
		} while (unprocessedData.size() < 1);
	}

	

	//  Graphics init
	GraphicsLibrary gl(SCREEN_X, SCREEN_Y);
	gl.init();

	// Input Init
	InputSystem is;
	is.init(&(gl));

	// load background
	gl.loadImage(FILE_PATH, BACKGROUND);
	gl.drawImage(BACKGROUND, 0, 0);
	gl.render();

	// `````````````````````````  main game loop  ``````````````````````````` 
	while (true)
	{
		is.Update(userIsCreator);

		//  UpdateGameWorld

		//  UpdateNetwork()
		//  |
		//  L HandleIncomingPackets()
		//  |
		//  L HandleOutgoingPackets()

		//  Render



		;
	}


	// ______________________________   Clean Up ______________________________

	connectionsOpen = false;

	listeningThread.join();
	SocketUtil::CleanUp();

	return 0;
}

