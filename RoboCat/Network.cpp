#include "Network.h"
#include <iostream>
#include "RoboCatPCH.h"

const std::string ASSET_PATH = "Images\\";

bool Network::init(GraphicsSystems* graphicsSystem, DeliveryNotificationManager* deliveryManager, std::string deanSprite, std::string amongSprite, std::string scottSprite, TCPSocketPtr liveSocket)
{
	mTCPSocket = liveSocket;
	mGameObjects = std::vector<std::pair<int, GameObject*>>();

	// Replication Data
	mGraphicsSystem = graphicsSystem;
	mDeliverymanager = deliveryManager;
	mDeanSprite = deanSprite;
	mAmongSprite = amongSprite;
	mScottSprite = scottSprite;

	return true;
}

void Network::cleanUp()
{
	(mTCPSocket)->~TCPSocket();
	SocketUtil::CleanUp();

	mGameObjects.clear();

	mGraphicsSystem->cleanup();
	delete mGraphicsSystem;
	mGraphicsSystem = nullptr;

	delete mDeliverymanager;
	mDeliverymanager = nullptr;
}

void Network::send(PacketType packetTypeHead, GameObject* object)
{
	//Variables
	OutputMemoryBitStream outBitStream;
	bool objectExists = true;

	if (mGameObjects.size() <= 0)
	{
		mGameObjects.push_back(std::pair<int, GameObject*>(object->getNetworkId(), object));
	}

	// Check if object already exists
	for (int i = 0; i < mGameObjects.size(); i++)
	{
		if (mGameObjects[i].first != object->getNetworkId())
		{
			objectExists = false;
		}
	}

	// Add object
	if (!objectExists)
	{
		mGameObjects.push_back(std::pair<int, GameObject*>(object->getNetworkId(), object));
	}

	// Write
	mDeliverymanager->WriteState(outBitStream);
	outBitStream.Write(packetTypeHead);
	outBitStream.Write(object->getNetworkId());
	outBitStream.Write(object->getClassId());

	// Packet Header
	switch (packetTypeHead)
	{
	case PacketType::CREATE_PACKET:

		outBitStream.Write(object->getPosition().first);
		outBitStream.Write(object->getPosition().second);

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
		}
		break;
	}
	// Send
	(mTCPSocket)->Send(outBitStream.GetBufferPtr(), outBitStream.GetByteLength());
}

PacketType Network::receive()
{
	// Variables
	char buffer[1024];
	int32_t byteReceive = (mTCPSocket)->Receive(buffer, 1024);

	if (byteReceive > 0)
	{
		// Variables
		InputMemoryBitStream inputBitStream = InputMemoryBitStream(buffer, byteReceive * 8);
		PacketType packetTypeHead;
		int networkID;
		ClassId objectID;

		// Read (READ IN SAME ORDER SENDING)
		mDeliverymanager->ReadAndProcessState(inputBitStream);
		inputBitStream.Read(packetTypeHead);
		inputBitStream.Read(networkID);
		inputBitStream.Read(objectID);

		switch (packetTypeHead)
		{
		case PacketType::CREATE_PACKET:
		{// Variables
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
			draw();
			return PacketType::CREATE_PACKET;
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
				
				return PacketType::DELETE_PACKET;

			}
			break;
		}
	}
}

void Network::draw()
{
	std::vector<std::pair<int, GameObject*>>::iterator it;
	for (it = mGameObjects.begin(); it != mGameObjects.end(); it++)
	{
		it->second->Draw();
	}
}
