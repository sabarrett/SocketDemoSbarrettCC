
#include "RoboCatPCH.h"
#include "InputSystem.h"
#include "GraphicsLibrary.h"
#include "Colour.h"
#include "GameObject.h"

const int MAX_OBJECT_COUNT = 100; //i think this might be needed i am unsure
std::string BACKGROUND_PATH = "..//..//Assets//Woods.png"; //I think

#if _WIN32

//maybe dont need these to be bools this time?
bool TCPServerSendMessages(TCPSocketPtr conn, SocketAddress address)
{
	char message[4096];
	char buffer[4096];

	int32_t bytesRead = conn->Receive(buffer, 4096);

	std::string msgReceived(buffer, bytesRead);
	LOG("Received message: %s", msgReceived.c_str());

	printf("%s", "Enter your message: ");
	scanf("%s", &message);

	if (strcmp(message, "&") == 0)
	{
		std::string msg("the peer has disconnected");
		conn->Send(msg.c_str(), msg.length());
		return false;
	}

	conn->Send(message, 4096);

	LOG("%s", "Sent message to peer");

	return true;
}

bool TCPClientSendMessages(TCPSocketPtr connSocket, SocketAddress address)
{
	//so i want to send out and receive all the different info each run through here
		//object count
		//object type
		//object position

	char message[4096];

	printf("%s", "Please enter a message to send:");

	scanf("%s", &message);

	if (strcmp(message, "&") == 0)
	{
		std::string msg("the peer has disconnected");
		connSocket->Send(msg.c_str(), msg.length());
		return false;
	}

	connSocket->Send(message, 4096);

	LOG("%s", "Sent message to peer");

	char buffer[4096];
	int32_t bytesRead = connSocket->Receive(buffer, 4096);

	std::string msgReceived(buffer, bytesRead);
	LOG("Received message: %s", msgReceived.c_str());

	return true;

}


void DoTCPServer()
{
	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("creating listening socket");
		ExitProcess(1);
	}

	LOG("%s", "created listening socket");

	SocketAddress a(INADDR_ANY, 8080);

	if (&a == nullptr)
	{
		SocketUtil::ReportError("creating server address");
		ExitProcess(1);
	}

	LOG("%s", "created socket address");

	if (listenSocket->Bind(a) != NO_ERROR)
	{
		SocketUtil::ReportError("binding socket");
		ExitProcess(1);
	}

	LOG("%s", "bound socket");

	// Call "Listen" to have the OS listen for connections on that socket
	if (listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("listening");
		ExitProcess(1);
	}

	LOG("%s", "socket listening");

	SocketAddress connAddr(INADDR_LOOPBACK, 8081);
	TCPSocketPtr conn;

	// This code isn't blocking anymore -- it'll run to the end of the program.
	//while (conn != nullptr)
	{
		conn = listenSocket->Accept(connAddr);
	}

	bool messageOnGoing = true;
	//messageOnGoing = TCPServerSendMessages(conn, connAddr);
}

void DoTCPClient()
{
	TCPSocketPtr connSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

	if (connSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listen socket");
		ExitProcess(1);
	}

	LOG("%s", "Created client socket");

	//SocketAddress a2(INADDR_LOOPBACK, 8080);
	// Listen only for connections on this machine
	//SocketAddressPtr a = SocketAddressFactory::CreateIPv4FromString(INADDR_ANY);
	SocketAddress a(INADDR_LOOPBACK, 8081);

	if (&a == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}

	LOG("%s", "Create socket address");

	// "Bind" that socket to the address we want to listen on
	if (connSocket->Bind(a) != NO_ERROR)
	{
		SocketUtil::ReportError("binding socket");
		ExitProcess(1);
	}

	LOG("%s", "bound socket");

	//SocketAddressPtr servAddress = SocketAddressFactory::CreateIPv4FromString(INADDR_ANY);
	SocketAddress servAddress(INADDR_LOOPBACK, 8080);

	if (&servAddress == nullptr)
	{
		SocketUtil::ReportError("creating server address");
		ExitProcess(1);
	}

	if (connSocket->Connect(servAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("connecting to server");
		ExitProcess(1);
	}

	LOG("%s", "connected to server!");
	
	std::string msg("someone has connected");
	connSocket->Send(msg.c_str(), msg.length());

	bool messageOnGoing = true;
	//messageOnGoing = TCPClientSendMessages(connSocket, servAddress);


	while (messageOnGoing == true)
	{
		//messageOnGoing = TCPClientSendMessages(connSocket, servAddress);
	}
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

	InputSystem mInputSystem; //do i need this to be a pointer and create it?
	GameObject mGameObjects[MAX_OBJECT_COUNT]; //possibly make this an array with MAX_OBJECT_COUNT
	GraphicsLibrary* mpGraphicsLibrary = new GraphicsLibrary(800, 800);

	bool activeConnection = true;
	bool isServer;
	int unitCount = 0; //use this to track changes for deleting/making units
	int pastUnitCount = 0;

	//something to start up the connection (info from assignment 1?)
	//then add the keys for making the objects spawn
	//each update send out the information about the object type and its position
		//then prepare to receive it as well?
	//use serialization ideas from previous save assignment last semester
		//i think it will have a lot of potential uses here

	isServer = StringUtils::GetCommandLineArg(1) == "server";

	if (isServer)
	{
		DoTCPServer(); //these maybe should function very differently
	}
	else
	{
		DoTCPClient();
	}

	mpGraphicsLibrary->init(BACKGROUND_PATH);
	//mpGraphicsLibrary->loadImage()

	while (activeConnection == true)
	{
		pastUnitCount = unitCount;
		mpGraphicsLibrary->render(); //render here or at the end?

		if (mInputSystem.getKeyboardInput() == KeyCode::S)
		{
			//spawn an object
			mGameObjects[unitCount].CreateObject();

			unitCount++;
		}

		if (mInputSystem.getKeyboardInput() == KeyCode::D)
		{
			//despawn an object
			int tmpRand;
			GameObject tmpObjects[MAX_OBJECT_COUNT];

			tmpRand = rand() % MAX_OBJECT_COUNT;

			mGameObjects[tmpRand].DeleteObject(); //could be unneeded

			for (int i = 0; i < unitCount; i++)
			{
				if (i != tmpRand)
				{
					tmpObjects[i] = mGameObjects[i];
				}
			}

			unitCount--;

			for (int i = 0; i < unitCount; i++)
			{
				mGameObjects[i] = tmpObjects[i];
			}
		}

		for (int i = 0; i < unitCount; i++)
		{
			mGameObjects[i].UpdatePosition();
			mGameObjects[i].DrawObjects();
		}

		//now i need to move and send this info

	}
	
	delete mpGraphicsLibrary;
	mpGraphicsLibrary = NULL; //is this needed?

	SocketUtil::CleanUp();

	return 0;
}