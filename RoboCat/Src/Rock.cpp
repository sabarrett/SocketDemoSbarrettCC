#include "Rock.h"
#include "RoboCatPCH.h"

Rock::Rock(const int networkID, GraphicsLibrary* graphicsLibrary)
	: GameObject(GameObjectType::ROCK, networkID, graphicsLibrary)
{

}

Rock::Rock(const int networkID, GraphicsLibrary* graphicsLibrary, pair<float, float> position, const std::string spriteIdentifier)
	: GameObject(GameObjectType::ROCK, networkID, graphicsLibrary, position, spriteIdentifier)
{

}

void Rock::HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager, const PacketSequenceNumber packetSequenceNum) const
{
	//Re-send packet
	inDeliveryNotificationManager->ResendPacket(mNetworkID, packetSequenceNum);
}

void Rock::HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager, const PacketSequenceNumber packetSequenceNum) const
{
	//Cool, already taken care of
}

Rock::~Rock()
{

}

void Rock::update()
{

}

void Rock::draw()
{
	//Draw sprite at mPosition
	pGraphicsLibrary->drawScaledImage(mSPRITE_IDENTIFIER, mPosition.first, mPosition.second, 0.25, 0.25);
}