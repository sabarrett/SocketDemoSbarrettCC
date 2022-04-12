#include "Network.h"
#include "SocketUtil.h"
#include "TCPSocket.h"
#include <iostream>

const std::string ASSET_PATH = "Images\\";

bool Network::init(GraphicsSystems* graphicsSystem, std::string deanSprite, std::string amongSprite, std::string scottSprite, TCPSocketPtr liveSocket)
{
	mTCPSocket = liveSocket;
	mGameObjects = std::vector<std::pair<int, GameObject*>>();
	//mGameObjects = std::vector<GameObject*>();

	// Replication Data
	mGraphicsSystem = graphicsSystem;
	mDeanSprite = deanSprite;
	mAmongSprite = amongSprite;
	mScottSprite = scottSprite;

	return true;
}

void Network::cleanUp()
{
	std::vector<std::pair<int, GameObject*>>::iterator it;
	//std::vector<GameObject*>::iterator it;

	for (it = mGameObjects.begin(); it != mGameObjects.end(); it++)
	{
		delete it->second;
		it->second = nullptr;
	}

	/*for (it = mGameObjects.begin(); it != mGameObjects.end(); it++)
	{
		delete* it;
	}
	mGameObjects.clear();*/

	(mTCPSocket)->~TCPSocket();
	SocketUtil::CleanUp();
}

void Network::send(PacketType packetTypeHead, GameObject* object)
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

		/*if (mGameObjects[i]->getNetworkId() != object->getNetworkId())
		{
			objectExists = false;
		}*/
	}

	// Add object
	if (objectExists)
	{
		mGameObjects.push_back(std::pair<int, GameObject*>(object->getNetworkId(), object));

		//mGameObjects.push_back(object);
	}

	// Write
	outBitStream.Write(packetTypeHead);
	outBitStream.Write(object->getNetworkId());
	outBitStream.Write(object->getClassId());
	/*outBitStream.Write(mGameObjects[object->getNetworkId()]->getNetworkId());
	outBitStream.Write(mGameObjects[object->getNetworkId()]->getClassId());*/

	// Packet Header
	switch (packetTypeHead)
	{
	case PacketType::CREATE_PACKET:
		outBitStream.Write(object->getPosition().first);
		outBitStream.Write(object->getPosition().second);
		/*outBitStream.Write(mGameObjects[object->getNetworkId()]->getPosition().first);
		outBitStream.Write(mGameObjects[object->getNetworkId()]->getPosition().second);*/
		break;
	case PacketType::DELETE_PACKET:
		if (mGameObjects.size() > 0)
		{
			std::vector<std::pair<int, GameObject*>>::iterator it;

			for (it = mGameObjects.begin(); it != mGameObjects.end(); it++)
			{
				if (it->first == object->getNetworkId() && it->second->getClassId() != ClassId::DEFAULT)
				{
					mGameObjects.erase(it);

					break;
				}
			}

			/*for (int i = 0; i < mGameObjects.size(); i++)
			{
				if (mGameObjects[i]->getNetworkId() == object->getNetworkId() && mGameObjects[i]->getClassId() != ClassId::DEFAULT)
				{
					mGameObjects.erase(mGameObjects.begin() + i);

					break;
				}
			}*/
		}
		break;
	}

	// Send
	outBitStreamTest = outBitStream;
	(mTCPSocket)->Send(outBitStream.GetBufferPtr(), outBitStream.GetBitLength());
}

void Network::receive()
{
	// Variables
	//char buffer[1024];
	int32_t byteReceive = (mTCPSocket)->Receive(outBitStreamTest.GetBufferPtr(), outBitStreamTest.GetBitLength()); // BUFFER VALUE FINE, INFINITE LOOP IN TCPSOCKET.CPP RECEIVE FUNCTION

	// Make sure there's something to read
	if (byteReceive > 0)
	{
		// Variables
		InputMemoryBitStream inputBitStream = InputMemoryBitStream(outBitStreamTest.GetBufferPtr(), outBitStreamTest.GetBitLength());
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
			{
				DeanSprite* newDean = new DeanSprite(networkID, std::pair<float, float>(posX, posY), ASSET_PATH + "dean_spritesCropped.png", mGraphicsSystem);
				mGameObjects.push_back(std::pair<int, GameObject*>(networkID, newDean));
				newDean = nullptr;

				/*DeanSprite* newDean = new DeanSprite(networkID, std::pair<float, float>(posX, posY), ASSET_PATH + "dean_spritesCropped.png", mGraphicsSystem);
				mGameObjects.push_back(newDean);
				newDean = nullptr;*/

				break;
			}
			case ClassId::AMONGUS:
			{
				AmongUs* newAmongUs = new AmongUs(networkID, std::pair<float, float>(posX, posY), ASSET_PATH + "amongUs.png", mGraphicsSystem);
				mGameObjects.push_back(std::pair<int, GameObject*>(networkID, newAmongUs));
				newAmongUs = nullptr;

				/*AmongUs* newAmongUs = new AmongUs(networkID, std::pair<float, float>(posX, posY), ASSET_PATH + "amongUs.png", mGraphicsSystem);
				mGameObjects.push_back(newAmongUs);
				newAmongUs = nullptr;*/

				break;
			}
			case ClassId::SCOTTSPRITE:
			{
				ScottSprite* newScott = new ScottSprite(networkID, std::pair<float, float>(posX, posY), ASSET_PATH + "SCOTT.png", mGraphicsSystem);
				mGameObjects.push_back(std::pair<int, GameObject*>(networkID, newScott));
				newScott = nullptr;

				/*ScottSprite* newScott = new ScottSprite(networkID, std::pair<float, float>(posX, posY), ASSET_PATH + "SCOTT.png", mGraphicsSystem);
				mGameObjects.push_back(newScott);
				newScott = nullptr;*/

				break;
			}
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


				/*for (int i = 0; i < mGameObjects.size(); i++)
				{
					if (mGameObjects[i]->getNetworkId() == networkID && mGameObjects[i]->getClassId() != ClassId::DEFAULT)
					{
						mGameObjects.erase(mGameObjects.begin() + i);

						break;
					}
				}*/
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

	/*for (int i = 0; i < mGameObjects.size(); i++)
	{
		mGameObjects[i]->Draw();
	}*/
}