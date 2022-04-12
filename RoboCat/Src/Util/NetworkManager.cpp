#include "Util/NetworkManager.h"
#include "Client/GameObject.h"
#include "Client/ColorChangerGameObject.h"
#include "Client/CollectableGameObject.h"

NetworkManager::NetworkManager() :
	mBytesSentThisFrame( 0 ),
	m_dropPacketChance( 0.f ),
	m_simulatedLatency( 0.f ),
	m_jitter(0.f)
{
}

NetworkManager::~NetworkManager()
{
}

bool NetworkManager::Init()
{
	// Create socket
	m_socketPtrTCP = SocketUtil::CreateTCPSocket(SocketAddressFamily::INET);
	m_socketPtrUDP = SocketUtil::CreateUDPSocket(SocketAddressFamily::INET);

	if (m_socketPtrTCP == nullptr)
	{
		SocketUtil::ReportError("Creating client socket");
		ExitProcess(1);
	}

	// Bind() - "Bind" socket -> bind to address:port - trying ports 64000-64100
	bool socketBound = false;
	int localPort = 64000;
	while (!socketBound && localPort <= 64100)
	{
		std::string myIPAddr = StringUtils::Sprintf("%s:%d", "127.0.0.1", localPort);
		SocketAddressPtr clientTCPAddress = SocketAddressFactory::CreateIPv4FromString(myIPAddr);
		if (clientTCPAddress == nullptr)
		{
			SocketUtil::ReportError("Creating client address");
		}
		if (m_socketPtrTCP->Bind(*clientTCPAddress) != NO_ERROR)
		{
			SocketUtil::ReportError("Binding TCP client socket");
			localPort++;
		}
		else
		{
			socketBound = true;
			LOG("Bound TCP client socket to %s", myIPAddr.c_str());
		}
	}
	socketBound = false;
	while (!socketBound && localPort <= 64100)
	{
		std::string myIPAddr = StringUtils::Sprintf("%s:%d", "127.0.0.1", localPort);
		SocketAddressPtr clientUDPAddress = SocketAddressFactory::CreateIPv4FromString(myIPAddr);
		if (clientUDPAddress == nullptr)
		{
			SocketUtil::ReportError("Creating client address");
		}
		if (m_socketPtrUDP->Bind(*clientUDPAddress) != NO_ERROR)
		{
			SocketUtil::ReportError("Binding UDP client socket");
			localPort++;
		}
		else
		{
			socketBound = true;
			m_socketPtrUDP->SetNonBlockingMode(true);
			LOG("Bound UDP client socket to %s", myIPAddr.c_str());
		}
	}
	return true;
}

void NetworkManager::ProcessIncomingPackets()
{
	ReadIncomingPacketsIntoQueue();

	ProcessQueuedPackets();
}

void NetworkManager::ReadIncomingPacketsIntoQueue()
{
	//should we just keep a static one?
	char packetMem[ 8192 ];
	int packetSize = sizeof( packetMem );
	InputMemoryBitStream inputStream( packetMem, packetSize * 8 );
	SocketAddress fromAddress;

	//keep reading until we don't have anything to read ( or we hit a max number that we'll process per frame )
	int receivedPacketCount = 0;
	int totalReadByteCount = 0;

	while( receivedPacketCount < kMaxPacketsPerFrameCount )
	{
		int readByteCount = m_socketPtrUDP->ReceiveFrom( packetMem, packetSize, fromAddress );
		if( readByteCount == 0 )
		{
			//nothing to read
			break;
		}
		else if( readByteCount == -WSAECONNRESET )
		{
			HandleConnectionReset( fromAddress );
		}
		else if( readByteCount > 0 )
		{
			inputStream.ResetToCapacity( readByteCount );
			++receivedPacketCount;
			totalReadByteCount += readByteCount;

			float receivedTime = Timing::sInstance.GetTimef();
			mPacketQueue.emplace(receivedTime, inputStream, fromAddress );
		}
	}

	if( totalReadByteCount > 0 )
	{
		mBytesReceivedPerSecond.UpdatePerSecond( static_cast< float >( totalReadByteCount ) );
	}
}

void NetworkManager::ProcessQueuedPackets()
{
	//Read queued receive packets
	while( !mPacketQueue.empty() )
	{
		ReceivedPacket& nextPacket = mPacketQueue.front();
		ProcessPacket( nextPacket.GetPacketBuffer(), nextPacket.GetFromAddress() );
		mPacketQueue.pop();
	}

	// Send queued sent packets
	auto it = mSentPacketsList.begin();
	while (it != mSentPacketsList.end())
	{
		// Once the scheduled send time has arrived, send the packet - simulating the jitter/latency.
		if (Timing::sInstance.GetTimef() > (*it)->GetSentTime())
		{
			int sentByteCount = m_socketPtrUDP->SendTo((*it)->GetPacketBuffer().GetBufferPtr(), (*it)->GetPacketBuffer().GetByteLength(), (*it)->GetFromAddress());
			if (sentByteCount > 0)
			{
				mBytesSentThisFrame += sentByteCount;
			}
			delete (*it);
			it = mSentPacketsList.erase(it);
		}
		else 
		{
			++it;
		}
	}
}

void NetworkManager::SendPacket(OutputMemoryBitStream& inOutputStream, const SocketAddress& inFromAddress )
{
	if (RoboMath::GetRandomFloat() >= m_dropPacketChance)
	{
		float simulatedSendTime = Timing::sInstance.GetTimef() + m_simulatedLatency + (m_jitter * RoboMath::GetRandomFloat());
		SentPacket* packet = new SentPacket(simulatedSendTime, inOutputStream, inFromAddress);
		mSentPacketsList.push_back(packet);
	}
}

NetworkManager::ReceivedPacket::ReceivedPacket( float inReceivedTime, InputMemoryBitStream& ioInputMemoryBitStream, const SocketAddress& inFromAddress ) :
	mReceivedTime( inReceivedTime ),
	mFromAddress( inFromAddress ),
	mPacketBuffer( ioInputMemoryBitStream )
{
}

NetworkManager::SentPacket::SentPacket(float inSentTime, OutputMemoryBitStream& OutputMemoryBitStream, const SocketAddress& inToAddress) :
	mSentTime(inSentTime),
	mToAddress(inToAddress),
	mPacketBuffer(OutputMemoryBitStream),
	sentFramesAgo(0)
{
}

GameObject* NetworkManager::CreateObject(int type, int UID, uint8_t textureID)
{
	GameObject* gameObject;
	switch (type)
	{
	case GAMEOBJECT_BASE:
		gameObject = new GameObject;
		break;
	case GAMEOBJECT_COLORCHANGER:
		gameObject = new ColorChangerGameObject;
		break;
	case GAMEOBJECT_COLLECTABLE:
		gameObject = new CollectableGameObject;
		break;
	default:
		gameObject = new GameObject;
		break;
	}

	gameObject->Init(UID, textureID);
	m_allObjects.push_back(gameObject);
	return gameObject;
}

bool NetworkManager::CheckIfObjectExistsWithUID(int UID)
{
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		if (m_allObjects[i]->GetUID() == UID)
		{
			return true;
		}
	}
	return false;
}

GameObject* NetworkManager::GetObjectByUID(int UID)
{
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		if (m_allObjects[i]->GetUID() == UID)
		{
			return m_allObjects[i];
		}
	}
	return nullptr;
}

void NetworkManager::DestroyObject(GameObject* gameObject)
{
	gameObject->CleanUp();
}