#include "RoboCatPCH.h"

#include "NetworkManager.h"

NetworkManager::NetworkManager()
{
	mpGraphicsLib = nullptr;
	mpTCPSocket = nullptr;
}

void NetworkManager::init(GraphicsLibrary* gLib, Colour p1Color, Colour p2Color)
{
	mpTCPSocket = new TCPSocketPtr;
	mObjectsVector = std::vector<std::pair<GameObjects*, int>>();

	mpGraphicsLib = gLib;
	mP1Color = p1Color;
	mP2Color = p2Color;
}

bool NetworkManager::initServer(std::string port)
{
	SocketUtil::StaticInit();

	TCPSocketPtr sock = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	if (sock == nullptr)
	{
		SocketUtil::ReportError("Creating Listening Socket\n");
		ExitProcess(1);
	}
	std::cout << "Listening Socket Created\n";

	SocketAddressPtr srvAddr = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:8080"); //change to be versatile
	if (srvAddr == nullptr)
	{
		SocketUtil::ReportError("Creating Listening Address\n");
		ExitProcess(1);
	}

	if (sock->Bind(*srvAddr) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding Listening Socket\n");
		ExitProcess(1);
	}
	std::cout << "Bound Listening Socket\n";

	if (sock->Listen() != NO_ERROR)
	{
		SocketUtil::ReportError("Listening on Socket\n");
		ExitProcess(1);
	}
	std::cout << "Listening Socket Working\n";

	std::cout << "Waiting on Connection.\n";

	sock->SetNonBlockingMode(false);
	SocketAddress clientAddr;
	TCPSocketPtr connectSock = sock->Accept(clientAddr);

	*mpTCPSocket = connectSock;
	std::cout << "Connected to: " << clientAddr.ToString() << "\n";

	if (mpTCPSocket != nullptr)
		return true;

	return false;

}

bool NetworkManager::connect(std::string IPAddr, std::string port)
{
	SocketUtil::StaticInit();

	TCPSocketPtr sock = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);

	if (sock == nullptr)
	{
		SocketUtil::ReportError("Client Socket");
		ExitProcess(1);
		return false;
	}
	std::cout << "Client Socket Created\n";

	string addr = "0.0.0.0:0";
	SocketAddressPtr cliAddr = SocketAddressFactory::CreateIPv4FromString(addr.c_str());
	if (cliAddr == nullptr)
	{
		SocketUtil::ReportError("Creating Client Address");
		ExitProcess(1);
		return false;
	}
	std::cout << "Client Address Working\n";

	if (sock->Bind(*cliAddr) != NO_ERROR)
	{
		SocketUtil::ReportError("Binding Client Socket");
		ExitProcess(1);
	}
	std::cout << "Bound Client Socket\n";

	SocketAddressPtr srvAddr = SocketAddressFactory::CreateIPv4FromString(IPAddr + ":" + port);
	if (srvAddr == nullptr)
	{
		SocketUtil::ReportError("Creating Server Address");
		ExitProcess(1);
	}
	std::cout << "Created Server Address\n";

	if (sock->Connect(*srvAddr) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to Server");
		system("pause");
		ExitProcess(1);
	}
	LOG("%s", "Connected");

	*mpTCPSocket = sock;

	if (mpTCPSocket != nullptr)
		return true;

	return false;

}

void NetworkManager::recieve()
{
	char buffer[1024];
	int32_t bytesRecieved = (*mpTCPSocket)->Receive(buffer, 1024);
	if (bytesRecieved > 0)
	{
		InputMemoryBitStream InMBStream = InputMemoryBitStream(buffer, 1024);
		//
		Packets packetHeader;
		InMBStream.Read(packetHeader);
		int networkID;
		InMBStream.Read(networkID);
		CurrentObject recieveType;
		InMBStream.Read(recieveType);
		//
		switch (packetHeader)
		{
		case Packets::PACKET_CREATE:
		{
			float posX;
			float posY;

			InMBStream.Read(posX);
			InMBStream.Read(posY);

			switch (recieveType)
			{
			case CurrentObject::BUBBLE:
				Bubble* newBubble;
				newBubble = new Bubble(mpGraphicsLib, networkID, mBubbleImgID, posX, posY);
				mObjectsVector.push_back(pair<GameObjects*, int>(newBubble, networkID));
				newBubble = nullptr;
				break;

			case CurrentObject::BOULDER:
				Boulder* newBoulder;
				newBoulder = new Boulder(mpGraphicsLib, networkID, mBoulderImgID, posX, posY);
				mObjectsVector.push_back(pair<GameObjects*, int>(newBoulder, networkID));
				newBoulder = nullptr;
				break;

			case CurrentObject::BEE:
				Bees* newBee;
				newBee = new Bees(mpGraphicsLib, networkID, mBeeImgID, posX, posY);
				mObjectsVector.push_back(pair<GameObjects*, int>(newBee, networkID));
				newBee = nullptr;
				break;
			}
			break;
		}
		case Packets::PACKET_UPDATE:
		{
			if (mObjectsVector[networkID].first != nullptr)
			{
				float posX;
				float posY;
				Colour playerColor;
				char direction;

				switch (recieveType)
				{
				case CurrentObject::BUBBLE: //HERE IS ISSUE
					Bubble* newBubble;
					newBubble= (Bubble*)mObjectsVector[networkID].first;
				
					InMBStream.Read(posX);
					InMBStream.Read(posY);
					//InMBStream.Read(playerColor);
				
					newBubble->setPosition(posX, posY);
					//newBubble->setPlayerColor(playerColor);
					break;
				
				case CurrentObject::BOULDER:
					InMBStream.Read(posX);
					InMBStream.Read(posY);
				
					mObjectsVector[networkID].first->setPosition(posX, posY);
					break;
				
				case CurrentObject::BEE:
					Bees* newBees;
					newBees = (Bees*)mObjectsVector[networkID].first;
				
					InMBStream.Read(posX);
					InMBStream.Read(posY);
					InMBStream.Read(direction);
				
					newBees->setPosition(posX, posY);
					newBees->setDirection(direction);
					break;
				}
			}
			else
				std::cout << "Broken broken";

			break;
		}

		default:
			return;
		}
	}
	else if (bytesRecieved == -10053 || bytesRecieved == -10054)
	{
		std::cout << "disconnected";
		exit(0);
	}
}

void NetworkManager::send(int networkID, Packets type)
{
	OutputMemoryBitStream OutMBStream;
	OutMBStream.Write(type);
	OutMBStream.Write(mObjectsVector[networkID].first->getNetworkID());
	OutMBStream.Write(mObjectsVector[networkID].first->getObjType());

	switch (type)
	{
	case Packets::PACKET_CREATE:
	case Packets::PACKET_UPDATE:
		switch (mObjectsVector[networkID].first->getObjType())
		{
		case CurrentObject::BUBBLE:
			Bubble* newBubble;
			newBubble = (Bubble*)mObjectsVector[networkID].first;
			OutMBStream.Write(newBubble->getPosition().first);
			OutMBStream.Write(newBubble->getPosition().second);
			//OutMBStream.Write(newBubble->getPlayerColor());
			break;

		case CurrentObject::BOULDER:
			OutMBStream.Write(mObjectsVector[networkID].first->getPosition().first);
			OutMBStream.Write(mObjectsVector[networkID].first->getPosition().second);
			break;

		case CurrentObject::BEE:
			Bees* newBee;
			newBee = (Bees*)mObjectsVector[networkID].first;
			OutMBStream.Write(newBee->getPosition().first);
			OutMBStream.Write(newBee->getPosition().second);
			OutMBStream.Write(newBee->getDirection());
			break;
		case CurrentObject::PLAYER:
			PlayerController* newpController;
			newpController = new PlayerController(networkID, mpGraphicsLib);
			mObjectsVector.push_back(pair<GameObjects*, int>(newpController, networkID));
			newpController = nullptr;
			break;
		}
		break;
	}

	(*mpTCPSocket)->Send(OutMBStream.GetBufferPtr(), OutMBStream.GetBitLength());
}

void NetworkManager::spawnObj(GameObjects* obj, int id)
{
	mObjectsVector.push_back(pair<GameObjects*, int>(obj, id));
}

void NetworkManager::updateObj()
{
	std::vector<std::pair<GameObjects*, int>>::iterator iter;
	for (iter = mObjectsVector.begin(); iter != mObjectsVector.end(); iter++)
	{
		iter->first->update();
	}
}

void NetworkManager::renderObj()
{
	std::vector<std::pair<GameObjects*, int>>::iterator iter;
	for (iter = mObjectsVector.begin(); iter != mObjectsVector.end(); iter++)
	{
		iter->first->draw();
	}
}