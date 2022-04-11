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

	UpdateBytesSentLastFrame();

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

			if( RoboMath::GetRandomFloat() >= m_dropPacketChance )
			//if (RoboMath::GetRandomFloat() >= 0)
			{
				float simulatedReceivedTime = Timing::sInstance.GetTimef() + m_simulatedLatency + (m_jitter * RoboMath::GetRandomFloat());
				//float simulatedReceivedTime = Timing::sInstance.GetTimef();
				mPacketQueue.emplace( simulatedReceivedTime, inputStream, fromAddress );
			}
		}
	}

	if( totalReadByteCount > 0 )
	{
		mBytesReceivedPerSecond.UpdatePerSecond( static_cast< float >( totalReadByteCount ) );
	}
}

void NetworkManager::ProcessQueuedPackets()
{
	//look at the front packet...
	while( !mPacketQueue.empty() )
	{
		ReceivedPacket& nextPacket = mPacketQueue.front();
		if( Timing::sInstance.GetTimef() > nextPacket.GetReceivedTime() )
		{
			ProcessPacket( nextPacket.GetPacketBuffer(), nextPacket.GetFromAddress() );
			mPacketQueue.pop();
		}
		else
		{
			break;
		}
	}

	//std::vector<int> indicesToDelete;
	//for (size_t i = 0; i < mSentPacketsList.size(); i++)
	//{
	//	if (Timing::sInstance.GetTimef() > mSentPacketsList[i].GetSentTime())
	//	{
	//		m_socketPtrUDP->SendTo(mSentPacketsList[i].GetPacketBuffer().GetBufferPtr(), mSentPacketsList[i].GetPacketBuffer().GetByteLength(), mSentPacketsList[i].GetFromAddress());
	//		indicesToDelete.push_back(i);
	//	}
	//}
	//for (size_t i = 0; i < indicesToDelete.size(); i++)
	//{
	//	//delete mSentPacketsList[indicesToDelete[i]];
	//	//mSentPacketsList.erase(mSentPacketsList.begin() + indicesToDelete[i]);
	//}
	//indicesToDelete.clear();
}

void NetworkManager::SendPacket(OutputMemoryBitStream& inOutputStream, const SocketAddress& inFromAddress )
{
	//if (RoboMath::GetRandomFloat() >= m_dropPacketChance)
	//{
	//	float simulatedSendTime = Timing::sInstance.GetTimef() + m_simulatedLatency + (m_jitter * RoboMath::GetRandomFloat());
	//	SentPacket packet = SentPacket(simulatedSendTime, inOutputStream, inFromAddress);
	//	SentPacket pocket(simulatedSendTime, inOutputStream, inFromAddress);
	//	mSentPacketsList.push_back(pocket);
	//	//m_socketPtrUDP->SendTo(mSentPacketsList[0]->GetPacketBuffer().GetBufferPtr(), mSentPacketsList[0]->GetPacketBuffer().GetByteLength(), mSentPacketsList[0]->GetFromAddress());
	//}

	int sentByteCount = m_socketPtrUDP->SendTo( inOutputStream.GetBufferPtr(), inOutputStream.GetByteLength(), inFromAddress );
	if (sentByteCount > 0)
	{
		mBytesSentThisFrame += sentByteCount;
	}
}

void NetworkManager::UpdateBytesSentLastFrame()
{
	if( mBytesSentThisFrame > 0 )
	{
		mBytesSentPerSecond.UpdatePerSecond( static_cast< float >( mBytesSentThisFrame ) );

		mBytesSentThisFrame = 0;
	}
}


NetworkManager::ReceivedPacket::ReceivedPacket( float inReceivedTime, InputMemoryBitStream& ioInputMemoryBitStream, const SocketAddress& inFromAddress ) :
	mReceivedTime( inReceivedTime ),
	mFromAddress( inFromAddress ),
	mPacketBuffer( ioInputMemoryBitStream )
{
}

NetworkManager::SentPacket::SentPacket(float inSentTime,OutputMemoryBitStream& OutputMemoryBitStream, const SocketAddress& inToAddress) :
	mSentTime(inSentTime),
	mToAddress(inToAddress),
	mPacketBuffer(OutputMemoryBitStream)
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