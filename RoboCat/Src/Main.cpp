
#include "RoboCatPCH.h"
#include "allegro5.h"
#include "allegro_primitives.h"
#include "allegro_color.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

#if _WIN32

void ThrowSocketError(std::string errorMsg)
{
	SocketUtil::ReportError(errorMsg.c_str());
	ExitProcess(1); // kill
}

void BradsTotallyOriginalServer()
{
	// create a socket to listen for input data
	//Create a SERVER

	// listening socket to take in new clients
	TCPSocketPtr listeningSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listeningSocket == nullptr)
		ThrowSocketError("Could not create socket: NullPtr");

	//create a ptr for the port
	SocketAddressPtr addressPtr = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:8080"); // a hard-coded port to bind to
	if (addressPtr == nullptr)
		ThrowSocketError("Could not create server address");

	// Bind listening socket to an address
	bool bindError = listeningSocket->Bind(*addressPtr) != NO_ERROR; // bind the socket and check if there is an error
	if (bindError)
		ThrowSocketError("Could not bind to address");

	// Listen on listeningSocket
	bool listenError = listeningSocket->Listen() != NO_ERROR;
	if (listenError)
		ThrowSocketError("Listen Error");

	// accept is a blocking function so it will halt the program execution until it recieves a incoming connection

	LOG("%s", "Waiting to accept connections...");

	SocketAddress incomingAddress;
	TCPSocketPtr incomingSocket = listeningSocket->Accept(incomingAddress);
	while (incomingSocket == nullptr) // if it didnt work the first time, try again
	{
		incomingSocket = listeningSocket->Accept(incomingAddress);
	}

	// CONECTION ACCEPTED WE HAVE CONTACT ==============================
	LOG("Request accepted from: %s", incomingAddress.ToString().c_str());

	// input username
	std::string username;
	std::cout << "Please Input Your Username: ";
	std::getline(std::cin, username);
	std::cout << "Waiting for other user to enter their username..." << std::endl;
	//send username
	incomingSocket->Send(username.c_str(), username.length());

	//receive username
	char buffer[4096];
	int32_t bytesReceived = incomingSocket->Receive(buffer, 4096); // get username
	while (bytesReceived < 0)
	{
		bytesReceived = incomingSocket->Receive(buffer, 4096);
	}

	std::string otherUsername(buffer, bytesReceived);
	std::cout << "Other user " << otherUsername.c_str() << " has joined the chatroom" << std::endl;

	bool exit = false;
	//send data
	std::thread SendThread([&incomingSocket, &exit]()
		{
			//send message
			std::string msg;
			while (!exit)
			{
				std::getline(std::cin, msg); //BLOCKS
				incomingSocket->Send(msg.c_str(), msg.length());

				if (msg == "/exit")
				{
					//incomingSocket->~TCPSocket();
					exit = true;
					std::cout << "connection terminated" << std::endl;
					break;
				}
			}
		});

	//receive data
	while (bytesReceived < 4096 && !exit)
	{
		bytesReceived = incomingSocket->Receive(buffer, 4096);
		if (bytesReceived <= 0)
		{
			exit = true;
			break;
		}
		std::string receivedMsg(buffer, bytesReceived);
		std::cout << otherUsername.c_str() << ": " << receivedMsg.c_str() << std::endl;
		if (receivedMsg == "/exit")
		{
			//incomingSocket->~TCPSocket();
			exit = true;
			std::cout << "connection terminated" << std::endl;
		}
	}
	incomingSocket->~TCPSocket();
}

void BradsLessOriginalClient()
{
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
		ThrowSocketError("Could not create client socket");

	std::string port;
	port = StringUtils::GetCommandLineArg(2);
	std::string address = StringUtils::Sprintf("127.0.0.1:%s", port.c_str());
	//std::string address = StringUtils::Sprintf("127.0.0.1:8081");
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
		ThrowSocketError("Creating client address");

	if (clientSocket->Bind(*clientAddress) != NO_ERROR)
		ThrowSocketError("Binding Client Socket");

	// Connect() -> Connect socket to remote host

	SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	if (serverAddress == nullptr)
		ThrowSocketError("Creating server address");

	if (clientSocket->Connect(*serverAddress) != NO_ERROR)
		ThrowSocketError("Connecting to server");

	//LOG("%s", "Connected to server!");
	/*
	// input username
	std::string username;
	std::cout << "Please Input Your Username: ";
	std::getline(std::cin, username);
	std::cout << "Waiting for other user to enter their username..." << std::endl;
	// send username
	clientSocket->Send(username.c_str(), username.length());
	*/

	/*
	// receive other username
	char otherUsernameBuffer[40];
	int32_t otherUsernameBytes = clientSocket->Receive(otherUsernameBuffer, 40);
	while (otherUsernameBytes < 0)
	{
		otherUsernameBytes = clientSocket->Receive(otherUsernameBuffer, 40);
	}
	std::string otherUsername(otherUsernameBuffer, otherUsernameBytes);
	std::cout << "Other user " << otherUsername.c_str() << " has joined the chatroom" << std::endl;
	*/
	bool exit = false;

	std::thread SendThread([&clientSocket, &exit]()
		{
			//Fulfill username request
			std::string msg;
			std::getline(std::cin, msg); //BLOCKS
			clientSocket->Send(msg.c_str(), msg.length());
			while (!exit)
			{
				std::getline(std::cin, msg);//BLOCKS
				clientSocket->Send(msg.c_str(), msg.length());

				if (msg == "/exit")
				{
					//clientSocket->~TCPSocket();
					exit = true;
					std::cout << "connection terminated" << std::endl;
					break;
				}
			}
		});

	char buffer[4096];
	int32_t bytesReceived = int32_t();
	while (bytesReceived < 4096 && !exit)
	{

		bytesReceived = clientSocket->Receive(buffer, 4096);
		if (bytesReceived <= 0)
		{
			exit = true;
			break;
		}
		std::string receivedMsg(buffer, bytesReceived);
		//std::cout << otherUsername.c_str() << ": " << receivedMsg.c_str() << std::endl;
		if (receivedMsg == "/exit")
		{
			//clientSocket->~TCPSocket();
			exit = true;
			std::cout << "connection terminated" << std::endl;
		}
	}
	clientSocket->~TCPSocket();
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

	ALLEGRO_DISPLAY* display;
	if (!al_init())
	{
		printf("fuck");
		return -1;
	}

	al_init_primitives_addon();
	al_install_keyboard();

	display = al_create_display(800, 600);

	ALLEGRO_COLOR color;
	color = al_map_rgb(255, 0, 0);

	al_draw_rectangle(10, 10, 100, 100, al_map_rgb(255, 0, 0), 2);
	al_flip_display();

	//system("pause");

	//socket stuff here
	SocketUtil::StaticInit(); // socket initialization

	/*
	
	*/
	//accept thread
	bool isServer = StringUtils::GetCommandLineArg(1) == "server"; // check if the command on the executable is 'server'
	if (isServer) // if it is the server
	{
		BradsTotallyOriginalServer();
		/*
		bool exit = false;
		thread AcceptThread([&exit, &listeningSocket]()
			{
				while (!exit)
				{
					SocketAddress clientAddress;
					TCPSocketPtr clientSocket = listeningSocket->Accept(clientAddress); // accept blocks				
				}

			});
		AcceptThread.join();
		*/
	}
	else
	{
		BradsLessOriginalClient();
	}

	SocketUtil::CleanUp(); // socket cleanup

	return 0;
}

