#include "NetworkManager.h"

NetworkManager::NetworkManager()
{
	mpGame = nullptr;
	mpSocket = nullptr;
}

NetworkManager::~NetworkManager()
{
}

void NetworkManager::initServer(std::string serverPort)
{
	SocketUtil::StaticInit();

	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (listenSocket == nullptr)
	{
		SocketUtil::ReportError("Creating listening socket");
		ExitProcess(1);
	}

	listenSocket->SetNonBlockingMode(true);

	std::cout << "Listening socket created" << endl;

	// Bind() - "Bind" socket -> tells OS we want to use a specific address

	SocketAddressPtr listenAddress = SocketAddressFactory::CreateIPv4FromString("127.0.0.1:8080");
	if (listenAddress == nullptr)
	{
		SocketUtil::ReportError("Creating listen address");
		ExitProcess(1);
	}

	if (listenSocket->Bind(*listenAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding listening socket");
		// This doesn't block!
		ExitProcess(1);
	}

	LOG("%s", "Bound listening socket");

	// Listen() - Listen on socket -> Non-blocking; tells OS we care about incoming connections on this socket
	if (listenSocket->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on listening socket");
		ExitProcess(1);
	}

	LOG("%s", "Listening on socket");

	// Accept() - Accept on socket -> Blocking; Waits for incoming connection and completes TCP handshake

	LOG("%s", "Waiting to accept connections...");
	SocketAddress incomingAddress;
	TCPSocketPtr connSocket = listenSocket->Accept(incomingAddress);
	while (connSocket == nullptr)
	{
		connSocket = listenSocket->Accept(incomingAddress);
		// SocketUtil::ReportError("Accepting connection");
		// ExitProcess(1);
	}

	LOG("Accepted connection from %s", incomingAddress.ToString().c_str());
}

void NetworkManager::connect(std::string serverPort, std::string clientPort)
{
}

void NetworkManager::createObject()
{
}

void NetworkManager::updateObject()
{
	mpGame->update();
}

void NetworkManager::destroyObject()
{

}

void NetworkManager::renderObject()
{
	mpGame->render();
}

void NetworkManager::sendData()
{
}

void NetworkManager::receiveData()
{
}
