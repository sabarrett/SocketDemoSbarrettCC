#include "RoboCatPCH.h"

#include "NetworkManager.h"
#include <iostream>

NetworkManager::NetworkManager()
{
	mpGraphicsLib = nullptr;
	mpTCPSocket = nullptr;
	mDropOdds = 0;
	mLastSentID = 0;
}

//~~~~~~~~~~~Networking  Code Processes~~~~~~~~~~~~~~~//
void NetworkManager::init(GraphicsLibrary* gLib, Colour p1Color, Colour p2Color, int dropOdds)
{
	mpTCPSocket = new TCPSocketPtr;
	mGameObjVector = std::vector<std::pair<GameObjects*, int>>();
	mpDeliveryNotifManager = new DeliveryNotificationManager(true, true, this);

	mpGraphicsLib = gLib;
	mP1Color = p1Color;
	mP2Color = p2Color;
	mDropOdds = dropOdds;
	mIsConnected = false;
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

	SocketAddressPtr srvAddr = SocketAddressFactory::CreateIPv4FromString("0.0.0.0:8080");
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

void NetworkManager::createConfirmPacket(int ID)
{
	OutputMemoryBitStream* OutMBStream = new OutputMemoryBitStream();
	OutMBStream->Write(CONFIRM);
	OutMBStream->Write(GameObjType::INVALID);
	OutMBStream->Write(ID);
	(*mpTCPSocket)->Send(OutMBStream->GetBufferPtr(), OutMBStream->GetByteLength());
}

bool NetworkManager::waitForConfirmPacket(int ID)
{
	if (mPendingResendPackets.size() > 0)
	{
		std::vector<pendingPacket>::iterator iter;
		for(iter = mPendingResendPackets.begin(); iter != mPendingResendPackets.end(); iter++ )
		{
			if (iter->id == ID)
			{
				mPendingResendPackets.erase(iter);
				return true;
			}
		}

		return false;
	}
}

void NetworkManager::processPacket(KeyCode key, string img, int player)
{
	switch (key)
	{
	case KeyCode::B:
	{
		float randPosX = rand() % (int)screenSizeX;
		float randPosY = 10.0;
		mServerNetworkID++;

		GameObjects* newBubble;
		newBubble = new Bubble(mpGraphicsLib, mServerNetworkID, img, randPosX, randPosY, player);

		spawnObj(newBubble, mServerNetworkID);
		send(mServerNetworkID, TypePacket::PACKET_CREATE);
		break;
	}
	case KeyCode::LEFT:
	{
		//make left bees
		float randPosY = rand() % (int)screenSizeY;
		float randPosX = 10.0;
		float randNum = rand() % 10;
		mServerNetworkID++;

		GameObjects* newBee;
		newBee = new Bees(mpGraphicsLib, mServerNetworkID, img, randPosX, randPosY, randNum);

		spawnObj(newBee, mServerNetworkID);
		send(mServerNetworkID, TypePacket::PACKET_CREATE);
		break;
	}
	case KeyCode::RIGHT: //maybe someday will be more bees
	{
		if (mServerNetworkID > 1)
		{
			send(mServerNetworkID, TypePacket::PACKET_DESTROY);
		}
		break;
	}
	case KeyCode::SPACE:
	{
		float randPosX = rand() % (int)screenSizeX;
		float randPosY = rand() % (int)screenSizeY;
		mServerNetworkID++;

		GameObjects* newBoulder;
		newBoulder = new Boulder(mpGraphicsLib, mServerNetworkID, img, randPosX, randPosY);

		spawnObj(newBoulder, mServerNetworkID);
		send(mServerNetworkID, TypePacket::PACKET_CREATE);
		break;
	}
	}

}

void NetworkManager::requestPacket(KeyCode key)
{
	OutputMemoryBitStream OutMBStream;
	GameObjType objType;
	TypePacket type;

	switch (key)
	{
	case KeyCode::B:
	{
		objType = BUBBLE;
		type = PACKET_CREATE;
		break;
	}
	case KeyCode::LEFT:
	{
		objType = BEE;
		type = PACKET_CREATE;
		break;
	}
	case KeyCode::RIGHT:
	{
		objType = BOULDER;
		type = PACKET_DESTROY;
		break;
	}
	case KeyCode::SPACE:
	{
		objType = BOULDER;
		type = PACKET_CREATE;
		break;
	}
	default:
		objType = INVALID;
		type = PACKET_UPDATE;
		break;
	}

	mClientPacketID++;
	OutMBStream.Write(type);
	OutMBStream.Write(objType);
	OutMBStream.Write(mClientPacketID);

	(*mpTCPSocket)->Send(OutMBStream.GetBufferPtr(), OutMBStream.GetByteLength());
}

PlayerController* NetworkManager::recieveInitFromServer(GraphicsLibrary* gLib)
{
	char buffer[1024];
	int32_t bytesRecieved = (*mpTCPSocket)->Receive(buffer, 1024);

	InputMemoryBitStream InMBStream = InputMemoryBitStream(buffer, 1024);
	TypePacket recievePacketType;
	InMBStream.Read(recievePacketType);

	PlayerController* playerServer = nullptr;

	switch (recievePacketType)
	{
	case TypePacket::PACKET_INIT:
		int objID;
		InMBStream.Read(objID);
		playerServer = new PlayerController(objID, gLib);
		spawnObj(playerServer, objID);
	}

	return playerServer;

}

void NetworkManager::recieve()
{
	char buffer[1024];
	int32_t bytesRecieved = (*mpTCPSocket)->Receive(buffer, 1024);
	KeyCode key;

	int randDrop = 0;
	if (mIsConnected)
	{
		randDrop = rand() % 101;
	}
	if (bytesRecieved > 0)
	{
		InputMemoryBitStream InMBStream = InputMemoryBitStream(buffer, 1024);
		TypePacket recievePacketType;
		InMBStream.Read(recievePacketType);

		if (randDrop < mDropOdds)
		{
			GameObjType recieveObjType;
			InMBStream.Read(recieveObjType);

			string imgID = "";

			if (mIsServer)
			{
				int confirmID;
				switch (recievePacketType)
				{
				case TypePacket::PACKET_CREATE:
					switch (recieveObjType)
					{
					case GameObjType::BUBBLE:
						InMBStream.Read(imgID);

						processPacket(KeyCode::B, "bubble_img", 1);
						break;

					case GameObjType::BEE:
						InMBStream.Read(imgID);

						processPacket(KeyCode::LEFT, "bee_img", 1);
						break;

					case GameObjType::BOULDER:
						InMBStream.Read(imgID);

						processPacket(KeyCode::SPACE, "boulder_img", 1);
						break;
					}
					break;

				case TypePacket::PACKET_DESTROY:
					if (mServerNetworkID > 1)
					{
						send(mServerNetworkID, TypePacket::PACKET_DESTROY);
					}
					break;

				case TypePacket::CONFIRM:
					InMBStream.Read(confirmID);
					waitForConfirmPacket(confirmID);
					break;
				}

				int packetID;
				InMBStream.Read(packetID);

			}

			else
			{
				int netID;
				InMBStream.Read(netID);
				GameObjects* obj;
				float posX = 0;
				float posY = 0;
				std::vector<std::pair<GameObjects*, int>>::iterator iter;

				switch (recievePacketType)
				{
					case TypePacket::PACKET_CREATE:
						switch (recieveObjType)
						{
							case GameObjType::BUBBLE:
								InMBStream.Read(imgID);
								InMBStream.Read(posX);
								InMBStream.Read(posY);

								GameObjects* newBubble;
								newBubble = new Bubble(mpGraphicsLib, netID, imgID, posX, posY, 0);
								spawnObj(newBubble, netID);
								createConfirmPacket(netID);
								newBubble = nullptr;
								break;

							case GameObjType::BEE:
								int num;
								InMBStream.Read(imgID);
								InMBStream.Read(posX);
								InMBStream.Read(posY);
								InMBStream.Read(num);

								GameObjects* newBee;
								newBee = new Bees(mpGraphicsLib, netID, imgID, posX, posY, num);
								spawnObj(newBee, netID);
								createConfirmPacket(netID);
								newBee = nullptr;
								break;

							case GameObjType::BOULDER:
								InMBStream.Read(imgID);
								InMBStream.Read(posX);
								InMBStream.Read(posY);

								GameObjects* newBoulder;
								newBoulder = new Boulder(mpGraphicsLib, netID, imgID, posX, posY);
								spawnObj(newBoulder, netID);
								createConfirmPacket(netID);
								newBoulder = nullptr;
								break;
						}
						break;

					case TypePacket::PACKET_UPDATE:
						int posX, posY;
						switch (recieveObjType)
						{
							case GameObjType::BUBBLE:
								InMBStream.Read(netID);
								InMBStream.Read(posY);

								mGameObjVector[netID].first->setPosY(posY);
								break;

							case GameObjType::BEE:
								InMBStream.Read(netID);
								InMBStream.Read(posX);

								mGameObjVector[netID].first->setPosX(posX);
								break;

							default:
								break;
						}
						break;

					case TypePacket::PACKET_DESTROY:
						int destroyID;
						InMBStream.Read(destroyID);
						for (iter = mGameObjVector.begin(); iter != mGameObjVector.end(); iter++)
						{
							if (iter->second == destroyID)
							{
								mGameObjVector.erase(iter);
								break;
							}
						}
						break;

					case TypePacket::CONFIRM:
						int packetID;
						InMBStream.Read(packetID);

					default:
						break;
				}
			}
		}
	}
	else if (bytesRecieved <= -10000)
	{
		std::cout << "disconnected";
		system("pause");
		exit(0);
	}
}

void NetworkManager::send(int networkID, TypePacket type)
{
	bool needsConfirm = false;
	OutputMemoryBitStream OutMBStream;
	OutMBStream.Write(type);
	OutMBStream.Write(mGameObjVector[networkID].first->getObjType());
	OutMBStream.Write(mGameObjVector[networkID].first->getNetworkID());

	switch (type)
	{
	case TypePacket::PACKET_CREATE:
		needsConfirm = true;
		switch (mGameObjVector[networkID].first->getObjType())
		{
			case GameObjType::BUBBLE:
				OutMBStream.Write(mGameObjVector[networkID].first->getImageID());
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().first);
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().second);
				break;

			case GameObjType::BEE:
				OutMBStream.Write(mGameObjVector[networkID].first->getImageID());
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().first);
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().second);
				OutMBStream.Write(mGameObjVector[networkID].first->getNum());
				break;

			case GameObjType::BOULDER:
				OutMBStream.Write(mGameObjVector[networkID].first->getImageID());
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().first);
				std::cout << mGameObjVector[networkID].first->getPosition().first << std::endl;
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().second);
				std::cout << mGameObjVector[networkID].first->getPosition().second << std::endl;
				break;

			default:
				break;
		}
		break;

	case TypePacket::PACKET_UPDATE:
		switch (mGameObjVector[networkID].first->getObjType())
		{
			case GameObjType::BUBBLE:
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().second);
				break;

			case GameObjType::BEE:
				OutMBStream.Write(mGameObjVector[networkID].first->getPosition().first);
				break;

			default:
				break;
		}
		break;

	case TypePacket::PACKET_DESTROY:
		needsConfirm = true;
		OutMBStream.Write(networkID);
		break;
	}

	(*mpTCPSocket)->Send(OutMBStream.GetBufferPtr(), OutMBStream.GetByteLength());

	if (needsConfirm)
	{
		pendingPacket newPacket;
		newPacket.data = OutMBStream.GetBufferPtr();//make a new buffer for data and copy from optMB to buffer, then when done with packet remember to delete buffer
		newPacket.dataSize = OutMBStream.GetByteLength();
		newPacket.id = mServerNetworkID;
		newPacket.objType = mGameObjVector[networkID].first->getObjType();
		mPendingResendPackets.push_back(newPacket);
	}
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

void NetworkManager::update(float deltaTime, float time)
{
	if (mPendingResendPackets.size() > 0)
	{
		if (mTimeTillResend <= 0.0f)
		{
			(*mpTCPSocket)->Send(mPendingResendPackets.front().data, mPendingResendPackets.front().dataSize);
			std::cout << mPendingResendPackets.front().objType;
			mTimeTillResend = 1.0f;
		}
		else
		{
			mTimeTillResend -= deltaTime;
		}
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

NetworkManager::~NetworkManager()
{
	mGameObjVector.clear();

	delete mpTCPSocket;
	mpTCPSocket = nullptr;

	SocketUtil::CleanUp();

	delete mpDeliveryNotifManager;
	mpDeliveryNotifManager = nullptr;
}