
#include "RoboCatPCH.h"
#include <thread>
#include "RoboCat/InputSystem.h"

#if _WIN32

const int SCREEN_SIZE_X = 1000;
const int SCREEN_SIZE_Y = 600;
const string FILE_PATH = "..\\common\\assets\\";
const string BACKGROUND_FILE = "steps.png";
const string HOMER_FILE = "HomerSimpson.png";
const string QUIMBY_FILE = "Mayor_Quimby.png";
const string DONUT_FILE = "Donut.png";

void TCPServer(UDPSocketPtr server)
{
	std::thread sendInput([&] {

	});

	std::thread update([&] {

	});

	sendInput.join();
	update.join();
}

void TCPClient(UDPSocketPtr client)
{
	std::thread sendInput([&] {

	});

	std::thread update([&] {

	});

	sendInput.join();
	update.join();
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

	UDPSocketPtr cliSock = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	UDPSocketPtr srvSock = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);

	GraphicsLib* gLib = new GraphicsLib(SCREEN_SIZE_X, SCREEN_SIZE_Y);
	gLib->init();
	gLib->loadImage(FILE_PATH + BACKGROUND_FILE, "background");
	gLib->drawImage("background", 0, 0);
	gLib->render();
		
	SocketAddressPtr srvAddr = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:9001");
	{
		SocketAddressPtr cliAddr = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:9000");
		if (cliAddr == nullptr)
		{
			SocketUtil::ReportError("Create client socket");
			ExitProcess(1);
		}
		cliSock->Bind(*cliAddr);
	}
	srvSock->Bind(*srvAddr);

	std::string msg("Hello server!");
	int bytesSent = cliSock->SendTo(msg.c_str(), msg.length(), *srvAddr);
	if (bytesSent <= 0)
	{
		SocketUtil::ReportError("Client Sendto");
	}
	std::cout << "Sent: " << bytesSent << std::endl;

	std::thread srvThread([&srvSock]() {
		char buffer[4096];
		SocketAddress fromAddr;
		int bytesReceived = srvSock->ReceiveFrom(buffer, 4096, fromAddr);
		if (bytesReceived <= 0)
		{
			SocketUtil::ReportError("Server ReceiveFrom");
			return;
		}
		std::string msg(buffer, bytesReceived);
		std::cout << "Received message from " << fromAddr.ToString() << ": " << msg << std::endl;
	});



	srvThread.join();

	SocketUtil::CleanUp();

	return 0;
}
