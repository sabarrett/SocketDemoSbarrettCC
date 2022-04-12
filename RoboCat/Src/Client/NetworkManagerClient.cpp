#include "Client/NetworkManagerClient.h"
#include "Client/GameObject.h"
#include "Client/ColorChangerGameObject.h"
#include "Client/CollectableGameObject.h"
#include "Client/Game.h"

namespace
{
	const float kTimeBetweenHellos = 1.f;
	const float kTimeBetweenInputPackets = 0.033f;
}

NetworkManagerClient::NetworkManagerClient() :
	mDeliveryNotificationManager(true, false),
	mLastRoundTripTime(0.f)
{
}

void NetworkManagerClient::ConnectToServer(std::string address, std::string port)
{
	// Connect() -> Connect socket to remote host
	std::string serverAddressString = StringUtils::Sprintf("%s:%s", address.c_str(), port.c_str());
	m_serverAddress = SocketAddressFactory::CreateIPv4FromString(serverAddressString);
	if (m_serverAddress == nullptr)
	{
		SocketUtil::ReportError("Creating server address");
		ExitProcess(1);
	}
	if (m_socketPtrTCP->Connect(*m_serverAddress) != NO_ERROR)
	{
		SocketUtil::ReportError("Connecting to server");
		ExitProcess(1);
	}
	LOG("%s", "Connected to server!");
	m_socketPtrTCP->SetNonBlockingMode(true);
}

void NetworkManagerClient::Init(Game* game)
{
	NetworkManager::Init();

	mAvgRoundTripTime = WeightedTimedMovingAverage(1.f);
	m_game = game;
}

void NetworkManagerClient::ReceiveTCP()
{
	char buffer[4096];
	int32_t bytesReceived = 1;
	while (bytesReceived > 0)
	{
		bytesReceived = m_socketPtrTCP->Receive(buffer, 4096);

		if (bytesReceived > 0)
		{
			InputMemoryBitStream inStream(buffer, bytesReceived);
			inStream.ResetToCapacity(bytesReceived);
			mPacketQueue.emplace(Timing::sInstance.GetTimef(), inStream, *m_serverAddress);
		}
	}
}
void NetworkManagerClient::ReadIncomingPacketsIntoQueue()
{
	ReceiveTCP();
	NetworkManager::ReadIncomingPacketsIntoQueue();
}

void NetworkManagerClient::ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
	SocketAddress fromAddress = inFromAddress; // <-- Declaring to hide an unreference warning from compiler - Keeping inFromAddress because its useful.

	uint32_t	packetType;
	inInputStream.Read(packetType);
	switch (packetType)
	{
	case PKTTYPE_WORLDSTATE:
		if (mDeliveryNotificationManager.ReadAndProcessState(inInputStream))
		{
			HandleWorldStatePacket(&inInputStream);
		}
		break;
	case PKTTYPE_SETPLAYERID: // MOVE
		int UID;
		inInputStream.Read(UID);
		m_myPlayerUID = UID;
		break;
	}
}


void NetworkManagerClient::SendOutgoingPackets()
{
	Inputs* input = m_game->GetInput();
	if (input->m_moveInput != 0)
	{
		OutputMemoryBitStream inputPacket;
		uint32_t OutPacketType = PKTTYPE_MOVEOBJECT;
		inputPacket.Write(OutPacketType);
		mDeliveryNotificationManager.WriteState(inputPacket);
		inputPacket.Write(m_myPlayerUID);
		inputPacket.Write(input->m_moveInput);
		SendPacket(inputPacket, *m_serverAddress);
	}

	// Send create object update
	if (input->m_leftClickDown)
	{
		OutputMemoryBitStream inputPacket;
		uint32_t OutPacketType = PKTTYPE_CREATEOBJECT;
		uint8_t textureID = 6;
		inputPacket.Write(OutPacketType);
		mDeliveryNotificationManager.WriteState(inputPacket);
		inputPacket.Write(textureID);
		inputPacket.Write(input->m_mouseX);
		inputPacket.Write(input->m_mouseY);
		SendPacket(inputPacket, *m_serverAddress);
	}
}


void NetworkManagerClient::HandleWorldStatePacket(InputMemoryBitStream* inStream)
{
	// Set objects to be destroyed if not contained in the following update
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		m_allObjects[i]->SetShouldDestroy(true);
	}

	// Packets always start with these three:
	int totalObjects;
	int UID;
	uint8_t objectType;
	inStream->Read(totalObjects);
	for (int i = 0; i < totalObjects; i++)
	{
		inStream->Read(UID);
		inStream->Read(objectType);
		switch (objectType)
		{
		case GAMEOBJECT_BASE:
		{
			GameObject* targetObject = GetObjectByUID(UID);
			if (targetObject == nullptr)
			{
				targetObject = CreateObject(objectType, UID, 0);
			}
			targetObject->Read(inStream);
			targetObject->SetShouldDestroy(false);
			break;
		}
		case GAMEOBJECT_COLORCHANGER:
		{
			ColorChangerGameObject* targetObject = (ColorChangerGameObject*)GetObjectByUID(UID);
			if (targetObject == nullptr)
			{
				targetObject = (ColorChangerGameObject*)CreateObject(objectType, UID, 0);
			}
			// Where other objects are very simple to read and write,
			// due to the need to getObjects by UID, this object's read is handled specially here.
			bool readMore;
			inStream->Read(readMore);
			if (readMore)
			{
				targetObject->Read(inStream);
				size_t count = 0;
				inStream->Read(count);
				for (size_t j = 0; j < count; j++)
				{
					int targetUID;
					inStream->Read(targetUID);
					targetObject->AddTargetObject(GetObjectByUID(targetUID));
				}
			}
			targetObject->SetShouldDestroy(false);
			break;
		}
		case GAMEOBJECT_COLLECTABLE:
		{
			CollectableGameObject* targetObject = (CollectableGameObject*)GetObjectByUID(UID);
			if (targetObject == nullptr)
			{
				targetObject = (CollectableGameObject*)CreateObject(objectType, UID, 0);
			}
			targetObject->Read(inStream);
			targetObject->SetShouldDestroy(false);
			break;
		}
		}
	}

	// Delete want-to-die objects
	for (size_t i = 0; i < m_allObjects.size(); i++)
	{
		if (m_allObjects[i]->GetShouldDestroy())
		{
			m_allObjects[i]->CleanUp();
			m_allObjects.erase(m_allObjects.begin() + i);
		}
	}
	m_game->CopyNetworkObjectsVector(m_allObjects);
}
