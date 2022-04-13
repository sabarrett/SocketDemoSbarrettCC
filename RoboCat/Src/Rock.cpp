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

void Rock::HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
	//Re-send create packet (only packet type we send for this GameObject)
	inDeliveryNotificationManager->ResendPacket(mNetworkID, PacketType::PACKET_CREATE);
}

void Rock::HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const
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