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