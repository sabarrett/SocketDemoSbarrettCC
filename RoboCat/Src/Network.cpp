#include "Network.h"
#include <iostream>

const std::string ASSET_PATH = "Images\\";

void Network::init(GraphicsSystems* graphicsSystem, std::string deanSprite, std::string amongSprite, std::string scottSprite)
{
	mTCPSocket = new TCPSocketPtr;
	mGameObjects = std::vector<std::pair<int, GameObject*>>();

	// Replication Data
	mGraphicsSystem = graphicsSystem;
	mDeanSprite = deanSprite;
	mAmongSprite = amongSprite;
	mScottSprite = scottSprite;
}

void Network::cleanUp()
{
	std::vector<std::pair<int, GameObject*>>::iterator it;

	for (it = mGameObjects.begin(); it != mGameObjects.end(); it++)
	{
		delete it->second;
		it->second = nullptr;
	}
	mGameObjects.clear();

	(*mTCPSocket)->~TCPSocket();
	SocketUtil::CleanUp();
}

void Network::send(int networkID, PacketType packetTypeHead, GameObject* object)
{
	//Variables
	OutputMemoryBitStream outBitStream;
	bool objectExists = true;

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
	outBitStream.Write(packetTypeHead);
	outBitStream.Write(mGameObjects[networkID].second->getNetworkId());
	outBitStream.Write(mGameObjects[networkID].second->getClassId());

	// Packet Header
	switch (packetTypeHead)
	{
	case PacketType::CREATE_PACKET:
		outBitStream.Write(mGameObjects[networkID].second->getPosition().first);
		outBitStream.Write(mGameObjects[networkID].second->getPosition().second);
		break;
	case PacketType::DELETE_PACKET:
		if (mGameObjects.size() > 0)
		{
			std::vector<std::pair<int, GameObject*>>::iterator it;

			for (it = mGameObjects.begin(); it != mGameObjects.end(); it++)
			{
				if (it->first == networkID && it->second->getClassId() != ClassId::DEFAULT)
				{
					mGameObjects.erase(it);

					break;
				}
			}
		}
		break;
	}

	// Send
	(*mTCPSocket)->Send(outBitStream.GetBufferPtr(), outBitStream.GetBitLength());
}

void Network::receive()
{
	// Variables
	char buffer[1024];
	int32_t byteReceive = (*mTCPSocket)->Receive(buffer, 1024);

	// Make sure there's something to read
	if (byteReceive > 0)
	{
		// Variables
		InputMemoryBitStream inputBitStream = InputMemoryBitStream(buffer, 1024);
		PacketType packetTypeHead;
		int networkID;
		ClassId objectID;

		// Read (READ IN SAME ORDER SENDING)
		inputBitStream.Read(packetTypeHead);
		inputBitStream.Read(networkID);
		inputBitStream.Read(objectID);

		switch (packetTypeHead)
		{
		case PacketType::CREATE_PACKET:
			// Variables
			float posX, posY;

			// Read
			inputBitStream.Read(posX);
			inputBitStream.Read(posY);

			switch (objectID)
			{
			case ClassId::DEANSPRITE:
				DeanSprite* newDean = new DeanSprite(networkID, std::pair<float, float>(posX, posY), ASSET_PATH + "dean_spritesCropped.png", mGraphicsSystem);
				mGameObjects.push_back(std::pair<int, GameObject*>(networkID, newDean));
				newDean = nullptr;

				break;
			case ClassId::AMONGUS:
				AmongUs* newAmongUs = new AmongUs(networkID, std::pair<float, float>(posX, posY), ASSET_PATH + "amongUs.png", mGraphicsSystem);
				mGameObjects.push_back(std::pair<int, GameObject*>(networkID, newAmongUs));
				newDean = nullptr;

				break;
			case ClassId::SCOTTSPRITE:
				ScottSprite* newScott = new ScottSprite(networkID, std::pair<float, float>(posX, posY), ASSET_PATH + "SCOTT.png", mGraphicsSystem);
				mGameObjects.push_back(std::pair<int, GameObject*>(networkID, newScott));
				newDean = nullptr;

				break;
			}

			break;
		case PacketType::DELETE_PACKET:
			if (mGameObjects.size() > 0)
			{
				std::vector<std::pair<int, GameObject*>>::iterator it;

				for (it = mGameObjects.begin(); it != mGameObjects.end(); it++)
				{
					if (it->first == networkID && it->second->getClassId() != ClassId::DEFAULT)
					{
						mGameObjects.erase(it);

						break;
					}
				}
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