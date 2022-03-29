
#include "RoboCatPCH.h"
#include "allegro5.h"
#include "allegro_primitives.h"
#include "allegro_color.h"

#include <thread>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
int windowWidth = 800;
int windowHeight = 600;
int FPS = 30;

#if _WIN32

class GameObject
{
public:
	int width;
	int height;
	int xPos;
	int yPos;
	void Read(InputMemoryBitStream& stream);
	void Write(OutputMemoryBitStream& stream) const;
	GameObject::GameObject(int newWidth, int newHeight, int startPosX, int startPosY);
	GameObject::GameObject();
	void UpdatePos(int xChange, int yChange);
	void Draw();
};


void GameObject::Read(InputMemoryBitStream& stream)
{
	stream.Read(width);
	stream.Read(height);
	stream.Read(xPos);
	stream.Read(yPos);
	
}

void GameObject::Write(OutputMemoryBitStream& stream) const
{
	stream.Write(width);
	stream.Write(height);
	stream.Write(xPos);
	stream.Write(yPos);
}

GameObject::GameObject(int newWidth, int newHeight, int startPosX, int startPosY)
{
	width = newWidth;
	height = newHeight;
	xPos = startPosX;
	yPos = startPosY;
}
GameObject::GameObject()
{
	width = 10;
	height = 10;
	xPos = rand() % 100 + 1;
	yPos = rand() % 100 + 1;
}

void GameObject::UpdatePos(int xChange, int yChange)
{
	//Update blockPosition
	/*
	int xStep = rand() % 2 + 1;
	//int xStep = 1;
	int yStep = rand() % 2 + 1;
	//int yStep = 1;
	int randNum = rand() % (2 - 1 + 1) + 1;
	if (randNum == 1)
	{
		xPos += xStep;
		yPos += yStep;
	}
	if (randNum == 2)
	{
		xPos -= xStep;
		yPos -= yStep;
	}
	if (xPos > windowWidth) xPos = 0;
	if (yPos > windowHeight) yPos = 0;
	if (xPos < 0) xPos = windowWidth;
	if (yPos < 0) yPos = windowHeight;
	*/
	xPos += xChange;
	yPos += yChange;
	if (xPos > windowWidth) xPos = 0;
	if (yPos > windowHeight) yPos = 0;
	if (xPos < 0) xPos = windowWidth;
	if (yPos < 0) yPos = windowHeight;
}

void GameObject::Draw()
{
	al_draw_rectangle(xPos, yPos, xPos + width, yPos + height, al_map_rgb(255, 0, 0), 2);
}

void ThrowSocketError(std::string errorMsg)
{
	SocketUtil::ReportError(errorMsg.c_str());
	ExitProcess(1); // kill
}



void BradsTotallyOriginalServer()
{

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

	LOG("%s", "Waiting to accept connections...");

	SocketAddress incomingAddress;
	TCPSocketPtr incomingSocket = listeningSocket->Accept(incomingAddress);
	while (incomingSocket == nullptr) // if it didnt work the first time, try again
	{
		incomingSocket = listeningSocket->Accept(incomingAddress);
	}

	// CONECTION ACCEPTED WE HAVE CONTACT ==============================
	LOG("Request accepted from: %s", incomingAddress.ToString().c_str());	

	//GAMELOOP
	bool exit = false;
	const int numObjects = 1;

	GameObject objects[numObjects];
	for (int j = 0; j < numObjects; j++)
	{
		objects[j] = GameObject(rand() % 100 + 1, rand() % 100 + 1, rand() % windowWidth + 1, rand() % windowHeight + 1);
	}
	//send data
	std::thread SendThread([&incomingSocket, &exit, &objects, &numObjects]()
		{
			//send message
			while (1)
			{
				OutputMemoryBitStream oStream = OutputMemoryBitStream();

				for (int i = 0; i < numObjects; i++)
				{
					objects[i].Write(oStream);
				}
				incomingSocket->Send(oStream.GetBufferPtr(), oStream.GetBitLength());
			}
		});
	//SendThread.join();

	GameObject inObj = GameObject();
	thread RecieveThread([&exit, &incomingSocket, &inObj]()
		{
			char buffer[4096];
			int32_t bytesReceived = int32_t(); // get username
			InputMemoryBitStream iStream = InputMemoryBitStream(buffer, 4096);
			while (1)
			{
				LOG("%s %s", "server game loop", exit);
				bytesReceived = incomingSocket->Receive(buffer, 4096);

				inObj.Read(iStream);

			}
			incomingSocket->~TCPSocket();
		});
	//SendThread.join();
	//UpdateThread.join();
	//RecieveThread.join();

	std::thread ServerUpdateThread([&objects, &numObjects, &exit, &inObj]()
		{
			ALLEGRO_DISPLAY* display;
			display = al_create_display(windowWidth, windowHeight);

			ALLEGRO_KEYBOARD_STATE keyState;
			ALLEGRO_EVENT_QUEUE* eventQueue = al_create_event_queue();
			al_register_event_source(eventQueue, al_get_keyboard_event_source());

			int xAxis = 0;
			int yAxis = 0;

			bool xMove = true;
			bool yMove = true;

			while (!exit) //GameLoop
			{
				//Get keyboardInput
				ALLEGRO_EVENT events;

				al_get_next_event(eventQueue, &events);
				if (events.type == ALLEGRO_EVENT_KEY_UP)
				{
					switch (events.keyboard.keycode)
					{
					case ALLEGRO_KEY_ESCAPE:
						exit = true;
					case ALLEGRO_KEY_UP:
						yMove = false;
						yAxis = 0;
					case ALLEGRO_KEY_DOWN:
						yMove = false;
						yAxis = 0;
					case ALLEGRO_KEY_RIGHT:
						xAxis = 0;
						xMove = false;
					case ALLEGRO_KEY_LEFT:
						xAxis = 0;
						xMove = false;
					}
				}
				if (events.type == ALLEGRO_EVENT_KEY_DOWN)
				{
					al_get_keyboard_state(&keyState);
					if (al_key_down(&keyState, ALLEGRO_KEY_UP))
					{
						yMove = true;
						yAxis = -1;
					}
					else if (al_key_down(&keyState, ALLEGRO_KEY_DOWN))
					{
						yMove = true;
						yAxis = 1;
					}
					else if (al_key_down(&keyState, ALLEGRO_KEY_RIGHT))
					{
						xAxis = 1;
						xMove = true;
					}

					else if (al_key_down(&keyState, ALLEGRO_KEY_LEFT))
					{
						xAxis = -1;
						xMove = true;
					}
				}

				for (int i = 0; i < numObjects; i++)
				{
					if (xMove || yMove)
					{
						objects[i].UpdatePos(xAxis, yAxis);
					}
					objects[i].Draw();
					inObj.Draw();
				}
				al_flip_display();
				al_clear_to_color(al_map_rgb(0, 0, 0));
			}
			//al_destroy_display(display);

		});
	//ServerUpdateThread.join();
	//receive data
	

}

void BradsLessOriginalClient()
{
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
		ThrowSocketError("Could not create client socket");

	//std::string port;
	//port = StringUtils::GetCommandLineArg(2);
	//std::string address = StringUtils::Sprintf("127.0.0.1:%s", port.c_str());
	std::string address = StringUtils::Sprintf("127.0.0.1:8081");
	SocketAddressPtr clientAddress = SocketAddressFactory::CreateIPv4FromString(address.c_str());
	if (clientAddress == nullptr)
		ThrowSocketError("Creating client address");

	if (clientSocket->Bind(*clientAddress) != NO_ERROR)
		ThrowSocketError("Binding Client Socket");

	SocketAddressPtr serverAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	if (serverAddress == nullptr)
		ThrowSocketError("Creating server address");

	if (clientSocket->Connect(*serverAddress) != NO_ERROR)
		ThrowSocketError("Connecting to server");

	//GameLoop
	bool exit = false;
	const int numObjects = 1;

	GameObject objects[numObjects];
	for (int j = 0; j < numObjects; j++)
	{
		objects[j] = GameObject(rand() % 100 + 1, rand() % 100 + 1, rand() % windowWidth + 1, rand() % windowHeight + 1);
	}

	std::thread ClientSendThread([&clientSocket, &exit, &objects, &numObjects]()
		{
			//send message
			while (1)
			{
				OutputMemoryBitStream oStream = OutputMemoryBitStream();

				for (int i = 0; i < numObjects; i++)
				{
					objects[i].Write(oStream);
				}
				clientSocket->Send(oStream.GetBufferPtr(), oStream.GetBitLength());
			}
			
		});
	//ClientSendThread.join();

	GameObject inObj = GameObject();
	thread ClientRecieveThread([&exit, &clientSocket, &numObjects,&inObj]()
		{
			char buffer[4096];
			InputMemoryBitStream iStream = InputMemoryBitStream(buffer, 4096);
			int32_t bytesReceived = int32_t();
			while (1)
			{
				LOG("%s", "server game loop");
				bytesReceived = clientSocket->Receive(buffer, 4096);

				inObj.Read(iStream);

				//al_draw_rectangle(inObj.xPos, inObj.yPos, inObj.xPos + inObj.width, inObj.yPos + inObj.height, al_map_rgb(255, 0, 0), 2);

			}
			clientSocket->~TCPSocket();
		});
	//SendThread.join();
	//UpdateThread.join();
	//ClientRecieveThread.join();
	
	std::thread ClientUpdateThread([&objects, &numObjects, &exit, &inObj]()
		{
			ALLEGRO_DISPLAY* display;
			display = al_create_display(windowWidth, windowHeight);

			ALLEGRO_KEYBOARD_STATE keyState;
			ALLEGRO_EVENT_QUEUE* eventQueue = al_create_event_queue();
			al_register_event_source(eventQueue, al_get_keyboard_event_source());

			int xAxis = 0;
			int yAxis = 0;

			bool xMove = true;
			bool yMove = true;

			while (!exit) //GameLoop
			{
				//Get keyboardInput
				ALLEGRO_EVENT events;

				al_get_next_event(eventQueue, &events);
				if (events.type == ALLEGRO_EVENT_KEY_UP)
				{
					switch (events.keyboard.keycode)
					{
					case ALLEGRO_KEY_ESCAPE:
						exit = true;
					case ALLEGRO_KEY_UP:
						yMove = false;
						yAxis = 0;
					case ALLEGRO_KEY_DOWN:
						yMove = false;
						yAxis = 0;
					case ALLEGRO_KEY_RIGHT:
						xAxis = 0;
						xMove = false;
					case ALLEGRO_KEY_LEFT:
						xAxis = 0;
						xMove = false;
					}
				}
				if (events.type == ALLEGRO_EVENT_KEY_DOWN)
				{
					al_get_keyboard_state(&keyState);
					if (al_key_down(&keyState, ALLEGRO_KEY_UP))
					{
						yMove = true;
						yAxis = -1;
					}
					else if (al_key_down(&keyState, ALLEGRO_KEY_DOWN))
					{
						yMove = true;
						yAxis = 1;
					}
					else if (al_key_down(&keyState, ALLEGRO_KEY_RIGHT))
					{
						xAxis = 1;
						xMove = true;
					}

					else if (al_key_down(&keyState, ALLEGRO_KEY_LEFT))
					{
						xAxis = -1;
						xMove = true;
					}
				}

				for (int i = 0; i < numObjects; i++)
				{
					if (xMove || yMove)
					{
						objects[i].UpdatePos(xAxis, yAxis);
					}
					objects[i].Draw();
					inObj.Draw();
				}
				al_flip_display();
				al_clear_to_color(al_map_rgb(0, 0, 0));
			}
			//al_destroy_display(display);
			//al_destroy_display(display);
		});
	//ClientUpdateThread.join();
	//receive data
	
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

	SocketUtil::StaticInit(); // socket initialization
	if (!al_init()) return -1;

	al_init_primitives_addon();
	al_install_keyboard();
	//accept thread
	bool isServer = StringUtils::GetCommandLineArg(1) == "server"; // check if the command on the executable is 'server'
	if (isServer) // if it is the server
	{
		BradsTotallyOriginalServer();
	}
	else
	{
		BradsLessOriginalClient();
	}

	SocketUtil::CleanUp(); // socket cleanup
	return 0;
}

