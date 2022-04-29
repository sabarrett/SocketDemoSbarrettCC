#include "RoboCatPCH.h"
#include "NetworkManager.h"


// inits
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

	if(listeningSocket->SetNonBlockingMode(true) != NO_ERROR)
		SocketUtil::ReportError("Error Setting To Non-blocking mode");

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
void NetworkManager::HandleListening(std::atomic<bool>* connectionsOpen, UDPSocketPtr& listeningSocket, SocketAddressPtr& addressRecievedFrom, priority_queue<pair<int, void*>>& unprocessedData)
{
	std::cout << "Listening Now!";

	while (connectionsOpen->load())
	{
		//LOG("%s", "In handleListening before Recieve\n");

		//  TODO, look into what length this buffer should be, make a better one
		char buffer[BUFFER_SIZE];
		int byteCount = listeningSocket->ReceiveFrom(buffer, BUFFER_SIZE, *(addressRecievedFrom));

		//LOG("%s", "In handleListening after Recieve\n");


		if (buffer != nullptr && byteCount > 0)
		{
			string msgRecieved(static_cast<char*>(buffer), BUFFER_SIZE);
			//LOG("Recieved message: %s", msgRecieved.c_str());
			unprocessedData.push(std::pair<int, void*>(time(0), buffer));
		}
	}
}

void NetworkManager::SetUpSending(int portToSendTo, int portUsedForSending, UDPSocketPtr& sendingSocket, SocketAddressPtr& sendingAddress)
{
	sendingSocket = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);
	if (sendingSocket == nullptr)
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
	while (sendingSocket->Bind(*a) != NO_ERROR)
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
	if ((sendingSocket->SendTo(msg.c_str(), BUFFER_SIZE, *sendingAddress)) < 0)
	{
		SocketUtil::ReportError("Sending First Message");
	}

	LOG("%s\n", "Starting message sent.");
}


// updates
bool NetworkManager::HandleIncomingInputPackets(priority_queue<pair<int, void*>>& unprocessedData, vector<JoinerInput>& joinerInputs, system_clock::time_point& lastConnectionTime)
{
	while(unprocessedData.size() > 0)
	{
		std::cout << "currently have " << unprocessedData.size() << " inputs left to process.\n";
		lastConnectionTime = system_clock::now();
		char* buffer = (char*)(unprocessedData.top().second);
		InputMemoryBitStream inStream = InputMemoryBitStream(buffer, BUFFER_SIZE);
		JoinerInput::Read(inStream, joinerInputs);
		unprocessedData.pop();
	}

	if (duration_cast<milliseconds>(system_clock::now() - lastConnectionTime).count() > CONNECTION_TIMOUT)
	{
		std::cout << "\n\nERROR: IT HAS BEEN " << CONNECTION_TIMOUT / 1000 << " SECONDS SINCE YOU HAVE RECIEVED DATA FROM JOINER. CLOSING GAME NOW.\n\n";
		return false;
	}

	return true;
}

bool NetworkManager::HandleOutgoingWorldStatePackets(WorldState& gameWorld, UDPSocketPtr &sendingSocket, SocketAddressPtr &sendingAddress, int delta)
{
	bool allGood = true;
	if(isDelayingAPacket)
	{ 
		currentDelayBetweenPacketSend -= delta;
		if (currentDelayBetweenPacketSend < 1)
		{
			isDelayingAPacket = false;
			std::cout << "delayed packet sent!\n";

			if (!sendingSocket->SendTo(delayedStream.GetBufferPtr(), delayedStream.GetByteLength(), *sendingAddress))
			{
				SocketUtil::ReportError("Sending outgoing delayed Packet");
				allGood = false;
			}
			delayedStream = OutputMemoryBitStream();
		}
	}
	
	if (((rand() % 100) + 1) > PACKET_DROP_CHANCE_PERCENT) // if we shouldn't drop a packet
	{
		if (!isDelayingAPacket && (rand() % 101) < PACKET_DELAY_CHANCE_PERCENT) // if we should delay a packet
		{
			isDelayingAPacket = true;
			currentDelayBetweenPacketSend = PACKET_DELAY_TIME;
			std::cout << "current packet deliberately delayed by " << PACKET_DELAY_TIME / 1000 << " seconds\n";
			gameWorld.Write(delayedStream);
		}
		else
		{
			gameWorld.Write(stream);
			if(!sendingSocket->SendTo(stream.GetBufferPtr(), stream.GetByteLength(), *sendingAddress))
			{
				SocketUtil::ReportError("Sending outgoing Packet");
				allGood = false;
			}
			stream = OutputMemoryBitStream();
		}
	}
	else
		std::cout << "deliberately dropped a packet\n";

	
	return allGood;
}


bool NetworkManager::HandleIncomingWorldStatePackets(WorldState& gameWorld, priority_queue<pair<int, void*>>& unprocessedDataPackets, system_clock::time_point& lastConnectionTime)
{
	if (unprocessedDataPackets.size() > 0)
	{
		lastConnectionTime = system_clock::now();
		if (unprocessedDataPackets.top().first > timeOfLastWorldState)// only world states newer than our current one
		{
			timeOfLastWorldState = unprocessedDataPackets.top().first;

			char* buffer = (char*)unprocessedDataPackets.top().second;
			InputMemoryBitStream inStream = InputMemoryBitStream(buffer, BUFFER_SIZE);
			gameWorld.Read(inStream);
		}

		// clearing the queue
		unprocessedDataPackets = priority_queue<pair<int, void*>>();
	}
	else if (duration_cast<milliseconds>(system_clock::now() - lastConnectionTime).count() > CONNECTION_TIMOUT)
	{
		std::cout << "\n\nERROR: IT HAS BEEN " << CONNECTION_TIMOUT/1000 << " SECONDS SINCE YOU HAVE RECIEVED DATA FROM CREATOR. CLOSING GAME NOW.\n\n";
		return false;
	}

	return true;
}

bool NetworkManager::HandleOutgoingInputPackets(vector<JoinerInput>& joinerInputs, UDPSocketPtr& sendingSocket, SocketAddressPtr& sendingAddress, system_clock::time_point& lastTimeOfSendingConnection, int delta)
{
	bool allGood = true;
	bool goodToClearInputs = true;

	// connection_timeout / 5 to give breathing room for the packet to send before dc, as well as if a few get dropped 
    if (duration_cast<milliseconds>(system_clock::now() - lastTimeOfSendingConnection).count() > CONNECTION_TIMOUT / 5)
	{
		//std::cout << "\n\nIT HAS BEEN " << CONNECTION_TIMOUT / 1000 / 5 << " SECONDS SINCE INPUT, SENDING A CONNECTION CONFIRMATION NOW.\n\n";
		joinerInputs.push_back(JoinerInput(JoinerInput::CONNECTION_CONFIRMATION_MESSAGE, Location{ 0,0 }));
	}


	if(isDelayingAPacket)
	{
		currentDelayBetweenPacketSend -= delta;
		if (currentDelayBetweenPacketSend < 1)
		{
			isDelayingAPacket = false;
			std::cout << "delayed packet sent!\n";

			if ((sendingSocket->SendTo(delayedStream.GetBufferPtr(), delayedStream.GetByteLength(), *sendingAddress)) < 0)
			{
				SocketUtil::ReportError("Sending outgoing delayed Packet");
				allGood = false;
			}
			delayedStream = OutputMemoryBitStream();

		}
	}

	if (joinerInputs.size() > 0)
	{
		lastTimeOfSendingConnection = system_clock::now();

		if (((rand() % 100) + 1) > PACKET_DROP_CHANCE_PERCENT) // if we shouldn't drop a packet
		{
			if (!isDelayingAPacket && ((rand() % 101) < PACKET_DELAY_CHANCE_PERCENT)) // if we should delay a packet
			{
				isDelayingAPacket = true;
				currentDelayBetweenPacketSend = PACKET_DELAY_TIME;

				JoinerInput::Write(delayedStream, std::ref(joinerInputs));

				std::cout << "current packet deliberately delayed by " << PACKET_DELAY_TIME / 1000 << " seconds\n";
			}
			else
			{
				
				JoinerInput::Write(stream, std::ref(joinerInputs));

				if ((sendingSocket->SendTo(stream.GetBufferPtr(), stream.GetByteLength(), *sendingAddress)) < 0)
				{
					SocketUtil::ReportError("Sending outgoingPacket");
					allGood = false;
				}
				stream = OutputMemoryBitStream();
			}
		}
		else
			std::cout << "deliberately dropped a packet\n";
	}



	joinerInputs.clear();

	return allGood;
}