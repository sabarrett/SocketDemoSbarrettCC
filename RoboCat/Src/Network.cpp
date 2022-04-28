#include "Network.h"
#include <iostream>
#include "RoboCatPCH.h"

const std::string ASSET_PATH = "Images\\";

Network::Network(GraphicsSystems* graphicsSystem, DeliveryNotificationManager* deliveryManager, std::string deanSprite, std::string amongSprite, std::string scottSprite, TCPSocketPtr liveSocket)
{
	mTCPSocket = liveSocket;
	mGameObjects = std::vector<std::pair<int, GameObject*>>();

	// Replication Data
	mGraphicsSystem = graphicsSystem;
	mDeliverymanager = deliveryManager;
	mDeanSprite = deanSprite;
	mAmongSprite = amongSprite;
	mScottSprite = scottSprite;
}

Network::~Network()
{
	(mTCPSocket)->~TCPSocket();
	SocketUtil::CleanUp();

	mGameObjects.clear();

	delete mGraphicsSystem;
	mGraphicsSystem = nullptr;

	// ASSIGNMENT 3
	delete mDeliverymanager;
	mDeliverymanager = nullptr;
}

void Network::Send(PacketType packetTypeHead, GameObject* object)
{
	//Variables

	std::shared_ptr<OutputMemoryBitStream> outBitStream = std::make_shared<OutputMemoryBitStream>();
	bool objectExists = false;

	// Check if object already exists
	for (int i = 0; i < mGameObjects.size(); i++)
	{
		if (mGameObjects[i].first == object->getNetworkId())
		{
			objectExists = true;
			break;
		}
	}

	// Add object
	if (!objectExists)
	{
		mGameObjects.push_back(std::pair<int, GameObject*>(object->getNetworkId(), object));
		objectExists = true;
	}

	// Write
	InFlightPacket* sentPacket = mDeliverymanager->WriteState(*outBitStream);
	outBitStream->Write(packetTypeHead);
	outBitStream->Write(object->getNetworkId());
	outBitStream->Write(object->getClassId());

	// Packet Header
	switch (packetTypeHead)
	{
	case PacketType::CREATE_PACKET:

		outBitStream->Write(object->getPosition().first);
		outBitStream->Write(object->getPosition().second);

		break;
	case PacketType::DELETE_PACKET:
		if (mGameObjects.size() > 0)
		{
			std::vector<std::pair<int, GameObject*>>::iterator it;

			for (it = mGameObjects.begin(); it != mGameObjects.end(); it++)
			{
				if (it->first == object->getNetworkId())
				{
					mGameObjects.erase(it);
					break;
				}
			}

			//for (int i = 0; i < mGameObjects.size(); i++)
			//{
			//	if (mGameObjects[i].first == object->getNetworkId())
			//	{
			//		mGameObjects[i].erase();
			//		break;
			//	}
			//}
		}
	case PacketType::UP_PACKET:
	{
		if (mGameObjects.size() > 0)
		{
			object->setPosition(object->getPosition().first, object->getPosition().second - 1);
			outBitStream->Write(object->getPosition().first);
			outBitStream->Write(object->getPosition().second);
		}

		break;
	}
	case PacketType::DOWN_PACKET:
	{
		if (mGameObjects.size() > 0)
		{
			object->setPosition(object->getPosition().first, object->getPosition().second + 1);
			outBitStream->Write(object->getPosition().first);
			outBitStream->Write(object->getPosition().second);
		}

		break;
	}
	case PacketType::LEFT_PACKET:
	{
		if (mGameObjects.size() > 0)
		{
			object->setPosition(object->getPosition().first - 1, object->getPosition().second);
			outBitStream->Write(object->getPosition().first);
			outBitStream->Write(object->getPosition().second);
		}

		break;
	}
	case PacketType::RIGHT_PACKET:
	{
		if (mGameObjects.size() > 0)
		{
			object->setPosition(object->getPosition().first + 1, object->getPosition().second);
			outBitStream->Write(object->getPosition().first);
			outBitStream->Write(object->getPosition().second);
		}

		break;
	}
		break;
	}

	std::shared_ptr<ReliableTransmissionData> relableData = std::make_shared<ReliableTransmissionData>(outBitStream, mTCPSocket);
	sentPacket->SetTransmissionData(object->getNetworkId(), relableData);

	float randomPacket = rand() % 100 + 1;
	if (randomPacket >= mDropPacketChance)
	{
		// Send
		(mTCPSocket)->Send(outBitStream->GetBufferPtr(), outBitStream->GetByteLength());
	}
}

void Network::Receive()
{
	// Variables
	char buffer[1024];
	int32_t byteReceive = (mTCPSocket)->Receive(buffer, 1024);

	if (byteReceive > 0)
	{
		//we made it, queue packet for later processing
		float simulatedReceivedTime =
			Timing::sInstance.GetTimef() +
			mSimulatedLatency +
			(rand() - 0.5f) *
			mDoubleSimulatedMaxJitter;
		//keep list sorted by simulated Receive time
		std::list<std::pair<float, InputMemoryBitStream>>::iterator it = mPacketList.end();
		if (mPacketList.empty())
		{
			mPacketList.push_back(std::make_pair(Timing::sInstance.GetTimef(), InputMemoryBitStream(buffer, byteReceive * 8)));
		}
		else
		{
			while (it != mPacketList.begin())
			{
				--it;
				if (it->first < simulatedReceivedTime)
				{
					//time comes after this element, so inc and break
					++it;
					break;
				}
			}
			mPacketList.emplace(it, std::make_pair(Timing::sInstance.GetTimef(), InputMemoryBitStream(buffer, byteReceive * 8)));
		}

		ProcessQueuedPackets();
	}
}

void Network::Draw()
{
	std::vector<std::pair<int, GameObject*>>::iterator it;
	for (it = mGameObjects.begin(); it != mGameObjects.end(); it++)
	{
		it->second->Draw();
	}
}

void Network::ProcessQueuedPackets()
{
	float currentTime = Timing::sInstance.GetTimef();
	while (!mPacketList.empty())
	{
		std::pair<float, InputMemoryBitStream> packet = mPacketList.front();
		//is it time to process this packet?
		if (currentTime > packet.first)
		{
			ProcessPacket(packet.second);
			mPacketList.pop_front();
		}
		else
		{
			break;
		}
	}
}

void Network::ProcessPacket(InputMemoryBitStream inputBitStream)
{
	PacketType packetTypeHead;
	int networkID;
	ClassId objectID;

	// Read(READ IN SAME ORDER SENDING)
	mDeliverymanager->ReadAndProcessState(inputBitStream);
	inputBitStream.Read(packetTypeHead);
	inputBitStream.Read(networkID);
	inputBitStream.Read(objectID);

	switch (packetTypeHead)
	{
	case PacketType::CREATE_PACKET:
	{
		// Variables
		float posX, posY;

		// Read
		inputBitStream.Read(posX);
		inputBitStream.Read(posY);

		switch (objectID)
		{
		case ClassId::DEANSPRITE:
		{
			DeanSprite* newDean = new DeanSprite(networkID, std::pair<float, float>(posX, posY), ASSET_PATH + "dean_spritesCropped.png", mGraphicsSystem);
			mGameObjects.push_back(std::pair<int, GameObject*>(networkID, newDean));
			newDean = nullptr;

			break;
		}
		case ClassId::AMONGUS:
		{
			AmongUs* newAmongUs = new AmongUs(networkID, std::pair<float, float>(posX, posY), ASSET_PATH + "amongUs.png", mGraphicsSystem);
			mGameObjects.push_back(std::pair<int, GameObject*>(networkID, newAmongUs));
			newAmongUs = nullptr;

			break;
		}
		case ClassId::SCOTTSPRITE:
		{
			ScottSprite* newScott = new ScottSprite(networkID, std::pair<float, float>(posX, posY), ASSET_PATH + "SCOTT.png", mGraphicsSystem);
			mGameObjects.push_back(std::pair<int, GameObject*>(networkID, newScott));
			newScott = nullptr;

			break;
		}
		}
		Draw();
	}

	break;
	case PacketType::DELETE_PACKET:
		if (mGameObjects.size() > 0)
		{
			std::vector<std::pair<int, GameObject*>>::iterator it;

			for (it = mGameObjects.begin(); it != mGameObjects.end(); it++)
			{
				if (it->first == networkID)
				{
					mGameObjects.erase(it);

					break;
				}
			}
		}
		break;
	case PacketType::UP_PACKET:
	{
		if (mGameObjects.size() > 0)
		{
			int currentXPos = mGameObjects.at(networkID).second->getPosition().first;
			int currentYPos = mGameObjects.at(networkID).second->getPosition().second;
			mGameObjects.at(networkID).second->setPosition(currentXPos, currentYPos - 1);
		}
		Draw();

		break;
	}
	case PacketType::DOWN_PACKET:
	{
		if (mGameObjects.size() > 0)
		{
			int currentXPos = mGameObjects.at(networkID).second->getPosition().first;
			int currentYPos = mGameObjects.at(networkID).second->getPosition().second;
			mGameObjects.at(networkID).second->setPosition(currentXPos, currentYPos + 1);
		}
		Draw();

		break;
	}
	case PacketType::LEFT_PACKET:
	{
		if (mGameObjects.size() > 0)
		{
			int currentXPos = mGameObjects.at(networkID).second->getPosition().first;
			int currentYPos = mGameObjects.at(networkID).second->getPosition().second;
			mGameObjects.at(networkID).second->setPosition(currentXPos - 1, currentYPos);
		}
		Draw();

		break;
	}
	case PacketType::RIGHT_PACKET:
	{
		if (mGameObjects.size() > 0)
		{
			int currentXPos = mGameObjects.at(networkID).second->getPosition().first;
			int currentYPos = mGameObjects.at(networkID).second->getPosition().second;
			mGameObjects.at(networkID).second->setPosition(currentXPos + 1, currentYPos);
		}
		Draw();

		break;
	}
	}
}
