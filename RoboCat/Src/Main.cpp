///
/// This code is based on the code provided by the instructor Scott Barrett, as well
/// as wrapper functions made by Adel Talhouk (the latter having header comments indicating such)
/// unless otherwise stated, the rest of the work was done by Brandon Boras (bb) and Nicholas Perell
/// 

// standard libraries
#include <thread>

// libraries provided to us
#include "RoboCatPCH.h"
#include "allegro_wrapper_functions-main/GraphicsLibrary.h"

// custom libraries


#if _WIN32


///
///          TODO
///  X - Synchronize game objects over a socket.
///  X - Create a two-player networked game with shared world state.
///  X - Use Memory Streams and different Packet Header types to synchronize the state between the two instances.
///  X - At least 10 synchronized objects in the world at once.
///  X - At least 3 different kinds of game objects.
///  X - Involve creating and destroying objects, which is then synchronized between clients.
///  X - Use TCP and UDP appropriately, (for deltas and world state respectively) 
///    X - if using UDP, there is a chance of things arriving out of order, so note the time of last remote update and send the time of the update with it, discarding old ones


const float SCREEN_X = 1000;
const float SCREEN_Y = 500;
const string FILE_PATH = "../images/";
const string BACKGROUND = "background.jpg";
const int CREATOR_PORT = 8080;
const int JOINER_PORT = 8100;

std::atomic<bool> closingConnections{ false };


void SetUpInitialListening(int& port, UDPSocketPtr& listeningSocket, SocketAddressPtr& listeningAddress)
{
		// here we can select which IPV we're using, and IPV6 is a bit wild for these purposes so, we go with IPV4
	listeningSocket = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	if (listeningSocket == nullptr)
	{
		SocketUtil::ReportError("Error Creating Listen Socket");
		ExitProcess(1);
	}
	LOG("%s", "created listening socket");

	listeningSocket->SetNonBlockingMode(true);

	listeningAddress = SocketAddressFactory::CreateIPv4FromString(("0.0.0.0:" + std::to_string(port++)));
	if (listeningAddress == nullptr)
	{
		SocketUtil::ReportError("Error creating listening address");
		ExitProcess(1);
	}

	LOG("%s", "binding the socket");
	while (listeningSocket->Bind(*listeningAddress) != NO_ERROR)
	{
		//LOG("%s", "port: 0.0.0.0:" + std::to_string(nextAvailablePort) + " taken, trying to use port: 0.0.0.0:" + std::to_string(nextAvailablePort + 1));
		listeningAddress = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:" + std::to_string(port));
	}

	//LOG("%s", "bound the socket");	
	//if (listeningSocket->ReceiveFrom() != NO_ERROR)
	//{
	//	SocketUtil::ReportError("Error listening with the socket");
	//	ExitProcess(1);
	//}

	LOG("Your port is %i", static_cast<int>(port));
	LOG("%s", "socket is now listening");
}


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
	bool userIsHosting;
	
	int listeningPort;
	UDPSocketPtr listeningSocket;
	SocketAddressPtr listeningAddress;

	SocketUtil::StaticInit();

	// pre-game menu
	std::cout << "Hello, would you like to create (c) or join (j) a match of this game?\n";
	do
	{
		std::cin >> userAnswer;
	} while (!(userAnswer == "c" || userAnswer == "C" || userAnswer == "j" || userAnswer == "J"));

	userIsHosting = (userAnswer == "c" || userAnswer == "C");

	
	if (userIsHosting)
	{
		std::cout << "Thank you for your answer, we will now wait for someone to join the game.\n";

		bool waitingForConnection = true;

		listeningPort = CREATOR_PORT;
		SetUpInitialListening(listeningPort, listeningSocket, listeningAddress);

		do
		{
			Sleep(500); // wait 0.5 second
			std::cout << '.';
		} while (waitingForConnection);

	}
	else // try to connect to a host
	{
		listeningPort = JOINER_PORT; 
	}

	//  Graphics init
	GraphicsLibrary gl(SCREEN_X, SCREEN_Y);
	gl.init();

	gl.loadImage(FILE_PATH + BACKGROUND, BACKGROUND);
	gl.drawImage(BACKGROUND, 0, 0);
	gl.render();

	// main game loop
	while (true)
	{
		//  Input
		//  UpdateGameWorld
		//  UpdateNetwork()
		//    HandleIncomingPackets()
		//    HandleOutgoingPackets()
		//  Render
		//  
		//  



		;
	}

	SocketUtil::CleanUp();

	return 0;
}
