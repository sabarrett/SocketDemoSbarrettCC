#include "RoboCatPCH.h"

#include "NetworkManager.h"

NetworkManager::NetworkManager()
{
	mpGraphicsLib = nullptr;
	mpTCPSocket = nullptr;
}

//~~~~~~~~~~~Networking  Code Processes~~~~~~~~~~~~~~~//
void NetworkManager::init(GraphicsLibrary* gLib, Colour p1Color, Colour p2Color)
{
	mpTCPSocket = new TCPSocketPtr;
	mGameObjVector = std::vector<std::pair<GameObjects*, int>>();

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
		TypePacket recievePacketType;
		InMBStream.Read(recievePacketType);

		int networkID;
		InMBStream.Read(networkID);
		if (mCurrentID < networkID)
			mCurrentID = networkID;

		GameObjType recieveObjType;
		InMBStream.Read(recieveObjType);

		switch (recievePacketType)
		{
			case TypePacket::PACKET_CREATE:
			{
				float posX;
				float posY;
				string imgID = "";
				int inPlayerID;
				int num;

				InMBStream.Read(posX);
				InMBStream.Read(posY);

				switch (recieveObjType)
				{
					case GameObjType::BUBBLE:
						InMBStream.Read(imgID);
						InMBStream.Read(inPlayerID);

						Bubble* newBubble;
						newBubble = new Bubble(mpGraphicsLib, networkID, imgID, posX, posY, inPlayerID);
						if (networkID < mGameObjVector.size() - 1) //compensate for dropped packets
							networkID = mGameObjVector.size();

						mGameObjVector.push_back(pair<GameObjects*, int>(newBubble, networkID));

						newBubble = nullptr;
						break;

				case GameObjType::BOULDER:
					InMBStream.Read(imgID);

					Boulder* newBoulder;
					newBoulder = new Boulder(mpGraphicsLib, networkID, imgID, posX, posY);
					mGameObjVector.push_back(pair<GameObjects*, int>(newBoulder, networkID));

					newBoulder = nullptr;
					break;

				case GameObjType::BEE:
					InMBStream.Read(imgID);
					InMBStream.Read(num);
				
					Bees* newBee;
					newBee = new Bees(mpGraphicsLib, networkID, imgID, posX, posY, num);
					mGameObjVector.push_back(pair<GameObjects*, int>(newBee, networkID));

					newBee = nullptr;
					break;

				case GameObjType::PLAYER:
					InMBStream.Read(inPlayerID);
					PlayerController* newPlayer;
					newPlayer = new PlayerController(networkID, mpGraphicsLib);
					mGameObjVector.push_back(pair<GameObjects*, int>(newPlayer, networkID));

					newPlayer = nullptr;
					break;
				}
				
			}

			case TypePacket::PACKET_UPDATE:
			{
				if (mGameObjVector[networkID].first != nullptr)
				{
					float posY;
					float posX;
					float num;
					//Colour playerColor;
					//char direction;

					switch (recieveObjType)
					{
					case GameObjType::BUBBLE: //HERE IS ISSUE
	
						InMBStream.Read(posY);
	
						mGameObjVector[networkID].first->setPosY(posY);
						break;
				
					case GameObjType::BOULDER:
						/*InMBStream.Read(posX);
						InMBStream.Read(posY);
				
						mGameObjVector[networkID].first->setPosition(posX, posY);*/
						break;
				
					case GameObjType::BEE:
						InMBStream.Read(posX);
						InMBStream.Read(num);

						mGameObjVector[networkID].first->setPosX(posX);
						break;

					case GameObjType::PLAYER:
						mGameObjVector[networkID].first->setPosition(0, 0); //placeholder content, player is just entity spawning as obj
						break;
					}
				}
				else
				{
					std::cout << "Broken broken";
					system("pause");
				}
				break;
			}

			case TypePacket::PACKET_DESTROY:
			{
				if (mGameObjVector.size() > 0)
				{
					std::vector<std::pair<GameObjects*, int>>::iterator iter;
					for (iter = mGameObjVector.begin(); iter != mGameObjVector.end(); iter++)
					{
						if (iter->first->getObjType() == GameObjType::BOULDER)
						{
							mGameObjVector.erase(iter);
							mCurrentID--;
							break;
						}
					}
					
				}
					break;
			}

			default:
				return;
		}
	}
	else if (bytesRecieved == -10053 || bytesRecieved == -10054)
	{
		std::cout << "disconnected";
		system("pause");
		exit(0);
	}
}

void NetworkManager::send(int networkID, TypePacket type)
{
	bool destroyed = false;
	OutputMemoryBitStream OutMBStream;
	OutMBStream.Write(type);
	OutMBStream.Write(mGameObjVector[networkID].first->getNetworkID());
	OutMBStream.Write(mGameObjVector[networkID].first->getObjType());

	switch (type)
	{
		case TypePacket::PACKET_CREATE:
		{
			switch (mGameObjVector[networkID].first->getObjType())
			{
			case GameObjType::BUBBLE:
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().first);
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().second);
				OutMBStream.Write(mGameObjVector[networkID].first->getImageID());
				OutMBStream.Write(mGameObjVector[networkID].first->getPlayerID()); //watch this too
				break;

			case GameObjType::BOULDER:
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().first);
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().second);
				OutMBStream.Write(mGameObjVector[networkID].first->getImageID());
				break;

			case GameObjType::BEE:
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().first);
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().second);
				OutMBStream.Write(mGameObjVector[networkID].first->getImageID());
				OutMBStream.Write(mGameObjVector[networkID].first->getNum());//watch for this
				break;

			case GameObjType::PLAYER:
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().first);
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().second);
				OutMBStream.Write(mGameObjVector[networkID].first->getNetworkID());
				break;
			}
			break;
		}

		case TypePacket::PACKET_UPDATE:
		{
			switch (mGameObjVector[networkID].first->getObjType())
			{
			case GameObjType::BUBBLE:
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().first);
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().second);
				break;

			case GameObjType::BOULDER:
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().first);
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().second);
				break;
			
			case GameObjType::BEE:
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().first);
				break;

			case GameObjType::PLAYER:
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().first);
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().second);
				OutMBStream.Write(mGameObjVector[networkID].first->getNetworkID());
				break;
			}
			break;
		}

		case TypePacket::PACKET_DESTROY:
		{	
			if (mGameObjVector.size() > 0)
			{
				std::vector<std::pair<GameObjects*, int>>::iterator iter;
				for (iter = mGameObjVector.begin(); iter != mGameObjVector.end(); iter++)
				{
					if (iter->first->getObjType() == GameObjType::BOULDER)
					{
						mGameObjVector.erase(iter);
						destroyed = true;
						break;
					}
				}

			}
			break;
		}

		default:
			return;
	}

	(*mpTCPSocket)->Send(OutMBStream.GetBufferPtr(), OutMBStream.GetBitLength());
	if (destroyed)
		mCurrentID--;
}

void NetworkManager::spawnObj(GameObjects* obj, int id)
{
	mGameObjVector.push_back(pair<GameObjects*, int>(obj, id));
}

void NetworkManager::updateObj()
{
	std::vector<std::pair<GameObjects*, int>>::iterator iter;
	for (iter = mGameObjVector.begin(); iter != mGameObjVector.end(); iter++)
	{
		iter->first->update();
	}
}

void NetworkManager::renderObj()
{
	std::vector<std::pair<GameObjects*, int>>::iterator iter;
	for (iter = mGameObjVector.begin(); iter != mGameObjVector.end(); iter++)
	{
		iter->first->draw();
	}
}