
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
int FRAMERATE = 16;

#if _WIN32

class RainParticle
{
public:
	RainParticle(float xStart, float yStart, float newRadius, float newR, float newG, float newB, float newA);
	RainParticle();
	void Read(InputMemoryBitStream& iStream);
	void Write(OutputMemoryBitStream& oStream) const;
	void Draw();
	void UpdatePos(float xChange, float yChange);

	vector<float> color;
	vector<float> position;
	float radius;
};

RainParticle::RainParticle(float xStart, float yStart, float newRadius, float newR, float newG, float newB, float newA)
{
	color.push_back(newR);
	color.push_back(newG);
	color.push_back(newB);
	color.push_back(newA);

	position.push_back(xStart);
	position.push_back(yStart);

	radius = newRadius;
}

RainParticle::RainParticle()
{
	float tempR, tempG, tempB, tempA, tempX, tempY;

	color.push_back(tempR);
	color.push_back(tempG);
	color.push_back(tempB);
	color.push_back(tempA);

	position.push_back(tempX);
	position.push_back(tempY);

	radius = float();
}

void RainParticle::Read(InputMemoryBitStream& iStream)
{
	iStream.Read(color[0]);
	iStream.Read(color[1]);
	iStream.Read(color[2]);
	iStream.Read(color[3]);

	iStream.Read(position[0]);
	iStream.Read(position[1]);

	iStream.Read(radius);
}

void RainParticle::Write(OutputMemoryBitStream& oStream) const
{
	oStream.Write(color[0]);
	oStream.Write(color[1]);
	oStream.Write(color[2]);
	oStream.Write(color[3]);

	oStream.Write(position[0]);
	oStream.Write(position[1]);

	oStream.Write(radius);
}

void RainParticle::Draw()
{
	al_draw_filled_circle(position[0], position[1], radius, al_map_rgba(color[0], color[1], color[2], color[3]));
}

void RainParticle::UpdatePos(float xChange, float yChange)
{
	position[0] += xChange;
	position[1] += yChange;
	if (position[0] > windowWidth) position[0] = 0;
	if (position[1] > windowHeight) position[1] = 0;
	if (position[0] < 0) position[0] = windowWidth;
	if (position[1] < 0) position[1] = windowHeight;
}

class CircleClass
{
public:
	CircleClass(int xStart, int yStart, float newRadius);
	CircleClass();
	void Read(InputMemoryBitStream& iStream);
	void Write(OutputMemoryBitStream& oStream) const;
	void Draw();
	void UpdatePos(int xChange, int yChange);

	vector<int> position;
	float radius;
};

void CircleClass::Read(InputMemoryBitStream& iStream)
{
	iStream.Read(position[0]);
	iStream.Read(position[1]);
	iStream.Read(radius);
}

void CircleClass::Write(OutputMemoryBitStream& oStream) const
{
	oStream.Write(position[0]);
	oStream.Write(position[1]);
	oStream.Write(radius);
}

void CircleClass::Draw()
{
	al_draw_filled_circle(position[0], position[1], radius, al_map_rgb(0, 255, 0));
}

CircleClass::CircleClass(int xStart, int yStart, float newRadius)
{
	position.push_back(xStart);
	position.push_back(yStart);
	radius = newRadius;
}

CircleClass::CircleClass()
{
	int xTemp = 0;
	int yTemp = 0;
	position.push_back(xTemp);
	position.push_back(yTemp);
	radius = 10;
}

void CircleClass::UpdatePos(int xChange, int yChange)
{
	position[0] += xChange;
	position[1] += yChange;
	if (position[0] > windowWidth) position[0] = 0;
	if (position[1] > windowHeight) position[1] = 0;
	if (position[0] < 0) position[0] = windowWidth;
	if (position[1] < 0) position[1] = windowHeight;
}

class RectangleObject
{
public:
	int width;
	int height;
	int xPos;
	int yPos;
	void Read(InputMemoryBitStream& stream);
	void Write(OutputMemoryBitStream& stream) const;
	RectangleObject(int newWidth, int newHeight, int startPosX, int startPosY);
	RectangleObject();
	void UpdatePos(int xChange, int yChange);
	void Draw();
	
};

void RectangleObject::Read(InputMemoryBitStream& stream)
{
	stream.Read(width);
	stream.Read(height);
	stream.Read(xPos);
	stream.Read(yPos);
}

void RectangleObject::Write(OutputMemoryBitStream& stream) const
{
	stream.Write(width);
	stream.Write(height);
	stream.Write(xPos);
	stream.Write(yPos);
}

RectangleObject::RectangleObject(int newWidth, int newHeight, int startPosX, int startPosY)
{
	width = newWidth;
	height = newHeight;
	xPos = startPosX;
	yPos = startPosY;
}
RectangleObject::RectangleObject()
{
	width = 10;
	height = 10;
	xPos = rand() % 100 + 1;
	yPos = rand() % 100 + 1;
}

void RectangleObject::UpdatePos(int xChange, int yChange)
{
	xPos += xChange;
	yPos += yChange;
	if (xPos > windowWidth) xPos = 0;
	if (yPos > windowHeight) yPos = 0;
	if (xPos < 0) xPos = windowWidth;
	if (yPos < 0) yPos = windowHeight;
}

void RectangleObject::Draw()
{
	al_draw_filled_rectangle(xPos, yPos, xPos + width, yPos + height, al_map_rgb(255, 0, 0));
}

void ThrowSocketError(std::string errorMsg)
{
	SocketUtil::ReportError(errorMsg.c_str());
	ExitProcess(1); // kill
}

ALLEGRO_TIMER* GiveMeATimer(float incr)
{
	ALLEGRO_TIMER* timer = al_create_timer(incr);
	if (timer == nullptr)
	{
		ERROR;
	}
	al_start_timer(timer);
	return timer;
}

TCPSocketPtr StartServer()
{
	TCPSocketPtr listeningSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listeningSocket == nullptr)
		ThrowSocketError("Could not create socket: NullPtr");

	SocketAddressPtr addressPtr = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:8080"); // a hard-coded port to bind to
	if (addressPtr == nullptr)
		ThrowSocketError("Could not create server address");

	bool bindError = listeningSocket->Bind(*addressPtr) != NO_ERROR; // bind the socket and check if there is an error
	if (bindError)
		ThrowSocketError("Could not bind to address");

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
	LOG("Request accepted from: %s", incomingAddress.ToString().c_str());

	return incomingSocket;
}

void BradsTotallyOriginalServer()
{
	TCPSocketPtr incomingSocket = StartServer();
	incomingSocket->SetNonBlockingMode(true);

	//Timer
	ALLEGRO_TIMER* timer = GiveMeATimer(0.001);
	int currentTime = al_get_timer_count(timer);
	int lastTime = 0;
	float deltaTime = 0;

	// Game Loop Variables
	bool exit = false;
	const int numObjects = 1;

	//Greate GameObjects
	CircleClass greenCircle[numObjects];
	for (int j = 0; j < numObjects; j++)
	{
		greenCircle[j] = CircleClass(100, 100, 20);
	}

	const int numDroplets = 10;
	RainParticle rain[numDroplets];
	for (int k = 0; k < numDroplets; k++)
	{
		rain[k] = RainParticle(rand() % windowWidth + 1, rand() % windowHeight + 1, rand() % 5 + 5, 0, 0, rand() % 254 + 1, rand() % 254 + 1);
	}

	RectangleObject inObjects[numObjects];
	
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

		//Timer
		currentTime = al_get_timer_count(timer);
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		//send Data
		OutputMemoryBitStream oStream = OutputMemoryBitStream();

		for (int i = 0; i < numObjects; i++)
		{
			greenCircle[i].Write(oStream);
		}

		for (int j = 0; j < numDroplets; j++)
		{
			rain[j].Write(oStream);
		}
		incomingSocket->Send(oStream.GetBufferPtr(), oStream.GetByteLength());

		//recieve Data
		char buffer[4096];
		int32_t bytesReceived = int32_t(); // get username
		InputMemoryBitStream iStream = InputMemoryBitStream(buffer, 4096);

		bytesReceived = incomingSocket->Receive(buffer, 4096);
		if (bytesReceived != -10035)
		{
			for (int i = 0; i < numObjects; i++)
			{
				inObjects[i].Read(iStream);
			}
		}

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
				greenCircle[i].UpdatePos(xAxis, yAxis);
			}
			inObjects[i].Draw();
			greenCircle[i].Draw();
		}
		for (int j = 0; j < numDroplets; j++)
		{
			rain[j].UpdatePos(0.1, 0.3);
			rain[j].Draw();
		}
		al_flip_display();
		al_clear_to_color(al_map_rgb(0,0,0));

		//wait for the frame to end
		int currentPeriod = al_get_timer_count(timer) - currentTime;
		while (currentPeriod < FRAMERATE)
		{
			currentPeriod = al_get_timer_count(timer) - currentTime;
		}
		if (currentTime < 5000)
			al_set_timer_count(timer, 0);
	}
	al_destroy_display(display);	
}

TCPSocketPtr StartClientConnection()
{
	TCPSocketPtr clientSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (clientSocket == nullptr)
		ThrowSocketError("Could not create client socket");

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

	return clientSocket;
}
void BradsLessOriginalClient()
{
	TCPSocketPtr clientSocket = StartClientConnection();
	clientSocket->SetNonBlockingMode(true);

	//timer	
	ALLEGRO_TIMER* timer = GiveMeATimer(0.001);
	int currentTime = al_get_timer_count(timer);
	int lastTime = 0;
	float deltaTime = 0;

	bool exit = false;
	const int numObjects = 1;

	//create Game Objects
	RectangleObject outRectangles[numObjects];

	for (int j = 0; j < numObjects; j++)
	{
		outRectangles[j] = RectangleObject(20, 20, rand() % windowWidth + 1, rand() % windowHeight + 1);
	}

	// Read In Objects
	CircleClass inObjects[numObjects];

	const int numDroplets = 10;
	RainParticle rain[numDroplets];

	// Main Game Loop
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
		//Timer
		currentTime = al_get_timer_count(timer);
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		//send Data
		OutputMemoryBitStream oStream = OutputMemoryBitStream();

		for (int i = 0; i < numObjects; i++)
		{
			outRectangles[i].Write(oStream);
		}
		clientSocket->Send(oStream.GetBufferPtr(), oStream.GetByteLength());

		// Recieve data
		char buffer[4096];
		InputMemoryBitStream iStream = InputMemoryBitStream(buffer, 4096);
		int32_t bytesReceived = int32_t();
		//LOG("%s", "Client game loop");
		bytesReceived = clientSocket->Receive(buffer, 4096);
		if (bytesReceived != -10035)
		{
			for (int i = 0; i < numObjects; i++)
			{
				inObjects[i].Read(iStream);
			}
			for (int j = 0; j < numDroplets; j++)
			{
				rain[j].Read(iStream);
			}
		}		

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
				outRectangles[i].UpdatePos(xAxis, yAxis);
			}
			outRectangles[i].Draw();
			inObjects[i].Draw();
		}
		for (int j = 0; j < numDroplets; j++)
		{
			rain[j].Draw();
		}
		al_flip_display();
		al_clear_to_color(al_map_rgb(0, 0, 0));

		//wait for the frame to end
		int currentPeriod = al_get_timer_count(timer) - currentTime;
		while (currentPeriod < FRAMERATE)
		{
			currentPeriod = al_get_timer_count(timer) - currentTime;
		}
		if (currentTime < 5000)
			al_set_timer_count(timer, 0);
	}
	al_destroy_display(display);

	
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

	SocketUtil::StaticInit(); 
	if (!al_init()) return -1;

	al_init_primitives_addon();

	al_install_keyboard();

	bool isServer = StringUtils::GetCommandLineArg(1) == "server"; // check if the command on the executable is 'server'
	if (isServer) 
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

