#include "RoboCatPCH.h"
#include "NetworkManager.h"

void NetworkManager::SetUpInitialListening(int& port, UDPSocketPtr& listeningSocket, SocketAddressPtr& listeningAddress)
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

void NetworkManager::HandleListening(bool& connectionsOpen, UDPSocketPtr listeningSocket, SocketAddressPtr addressRecievedFrom, vector<std::pair<int, void*>>& unprocessedData)
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


void NetworkManager::SetUpSending(int portToSendTo, int portUsedForSending, UDPSocketPtr sendingSocket, SocketAddressPtr sendingAddress)
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



bool NetworkManager::HandleUnprocessedIncomingPackets()
{
	std::cout << "HandlingIncoming";
	return true;
}

bool NetworkManager::HandleOutGoingWorldState()
{
	std::cout << "HandlingOutgoing";
	return true;
}


