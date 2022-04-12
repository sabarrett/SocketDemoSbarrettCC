#include "RoboCatPCH.h"
#include "GameData.h"

GameData* GameData::instance_ = nullptr;;

GameData* GameData::getInstance()
{
    if (instance_ == nullptr) {
        instance_ = new GameData();
    }
    return instance_;
}

void GameData::SetSocket(TCPSocketPtr newSocket)
{
    inSocket = newSocket;
}

void GameData::resendAllEffects()
{
    for (auto& effect : effectsVector)
    {
        effect->SendEffect(inSocket);
    }
}