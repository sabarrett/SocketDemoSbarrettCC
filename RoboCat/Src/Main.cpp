///
/// This code is based on the code provided by the instructor Scott Barrett, as well
/// as wrapper functions made by Adel Talhouk (the latter having header comments indicating such)
/// unless otherwise stated, the rest of the work was done by Brandon Boras (bb) and Nicholas Perell
/// 

// standard libraries


// libraries provided to us
#include "RoboCatPCH.h"
#include "allegro_wrapper_functions-main/GraphicsLibrary.h"


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
///  X - when user wants to quit, set  closingConnections = true; (to clean things up)

// can probably be bigger since we're using UDP
const int BUFFER_SIZE = 4096;

const float SCREEN_X = 1000;
const float SCREEN_Y = 500;
const string FILE_PATH = "../images/";
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
	LOG("%s", "created listening socket");

	// lol true sets it to blocking mode, idk why
	if(listeningSocket->SetNonBlockingMode(true) != NO_ERROR)   
		SocketUtil::ReportError("Error Setting To Blocking mode");

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
void HandleListening(bool& closingConnections, UDPSocketPtr listeningSocket, SocketAddressPtr addressRecievedFrom, std::stack<void*>& unprocessedData, bool& waitingForFirstMessage)
{
	std::cout << "Listening Now!"; 
	while (!closingConnections)
	{
		void* recievedData = nullptr;
		listeningSocket->ReceiveFrom(recievedData, BUFFER_SIZE, *(addressRecievedFrom));
		
		if (recievedData != nullptr)
		{
			string msgRecieved(static_cast<char*>(recievedData), BUFFER_SIZE);
			LOG("Recieved message: %s", msgRecieved.c_str());
			waitingForFirstMessage = false;
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
	LOG("%s", "created connection socket");

	SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString(("127.0.0.1:" + std::to_string(portUsedForSending)));
	//SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:9001");
	if (a == nullptr)
	{
		SocketUtil::ReportError("Error creating sending address");
		ExitProcess(1);
	}
	LOG("%s", "created connection socket address");

	LOG("%s", "binding the connection socket");
	while (newSendingSocket->Bind(*a) != NO_ERROR)
	{
		//LOG("%s", "port: 127.0.0.1:" + std::to_string(nextAvailablePort) + " taken, using port: 127.0.0.1:" + std::to_string(nextAvailablePort));
		a = SocketAddressFactory::CreateIPv4FromString(("127.0.0.1:" + std::to_string(++portUsedForSending)));
	}
	LOG("%s", "finished binding the connection socket");


	LOG("%s%i", "127.0.0.1:", portToSendTo);
    sendingAddress = SocketAddressFactory::CreateIPv4FromString(("127.0.0.1:" + std::to_string(portToSendTo)));  // this has to match the server's address, and it MUST NOT match client's own
		//SocketAddressPtr foreignAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8000");  // this has to match the server's address, and it MUST NOT match client's own
	if (sendingAddress == nullptr)
	{
		SocketUtil::ReportError("Error	creating foreign listener address");
		ExitProcess(1);
	}

	string msg("Hello, server! How are you today?");
	//std::string msg(std::to_string(listeningPort));
	if (newSendingSocket->SendTo(msg.c_str(), msg.length(), *sendingAddress) < 0);
	{
		SocketUtil::ReportError("Error Sending First Message");
	}

	LOG("%s", "sent message");
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
	bool userIsHosting;
	bool closingConnections = false;
	int listeningPort;
	std::thread listeningThread;
	UDPSocketPtr listeningSocket;
	SocketAddressPtr listeningAddress;

	UDPSocketPtr sendingSocket;
	SocketAddressPtr sendingAddress;

	SocketAddressPtr addressToSendTo;

	std::stack<void*> unprocessedData;

	//--------------------- intialization window -------------------------------

	SocketUtil::StaticInit();

	// pre-game menu
	std::cout << "Hello, would you like to create (c) or join (j) a match of this game?\n";
	do
	{
		std::cin >> userAnswer;
		std::cin.clear();
	} while (!(userAnswer == "c" || userAnswer == "C" || userAnswer == "j" || userAnswer == "J"));

	userIsHosting = (userAnswer == "c" || userAnswer == "C");

	
	if (userIsHosting)
	{
		std::cout << "Thank you for your answer, we will now wait for someone to join the game.\n";

		bool waitingForConnection = true;


		listeningPort = CREATOR_PORT;
		SetUpInitialListening(listeningPort, listeningSocket, listeningAddress);
		
		addressToSendTo = SocketAddressFactory::CreateIPv4FromString(("0.0.0.0:" + std::to_string(JOINER_PORT+1)));
		HandleListening(closingConnections, listeningSocket, addressToSendTo, unprocessedData, waitingForConnection);
		//something about this thread is cursed, throws errors in the actual implementation of thread
		//listeningThread = thread(HandleListening, closingConnections, listeningSocket, listeningAddress, unprocessedData, waitingForConnection);

		do
		{
			Sleep(500); // wait 0.5 second
			std::cout << '.';
		} while (waitingForConnection);


		// set up sending to joiner here

	}
	else // try to connect to a host
	{
		listeningPort = JOINER_PORT; 
		SetUpSending(CREATOR_PORT+1, listeningPort, sendingSocket, sendingAddress);
	}

	//  Graphics init
	GraphicsLibrary gl(SCREEN_X, SCREEN_Y);
	gl.init();

	gl.loadImage(FILE_PATH + BACKGROUND, BACKGROUND);
	gl.drawImage(BACKGROUND, 0, 0);
	gl.render();

	// `````````````````````````  main game loop  ``````````````````````````` 
	while (true)
	{
		//  Input

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

	listeningThread.join();
	SocketUtil::CleanUp();

	return 0;
}
