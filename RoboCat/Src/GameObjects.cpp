#include "GameObjects.h"

GameObject::GameObject(int networkId, std::pair<float, float> position, GraphicsSystems* graphics, std::string imageFilePath, ClassId id)
{
	mNetworkId = networkId;
	mPosition = position;
	mId = id;
	mGraphics = graphics;
	mpBitmap = al_load_bitmap(imageFilePath.c_str());
}

GameObject::~GameObject()
{
	mGraphics->cleanup();
	delete mGraphics;
	mGraphics = nullptr;
	
	al_destroy_bitmap(mpBitmap);
	mpBitmap = nullptr;
}

void GameObject::Draw()
{
	mGraphics->draw(mPosition.first - (al_get_bitmap_width(mpBitmap) * 0.5), mPosition.second - (al_get_bitmap_height(mpBitmap) * 0.5), mpBitmap);
	//mGraphics->flip();
}

DeanSprite::DeanSprite(int networkId, std::pair<float, float> position, std::string imageFilePath, GraphicsSystems* graphics)
	:GameObject(networkId, position, graphics, imageFilePath, DEANSPRITE)
{
	
	
}

AmongUs::AmongUs(int networkId, std::pair<float, float> position, std::string imageFilePath, GraphicsSystems* graphics)
	: GameObject(networkId, position, graphics, imageFilePath, AMONGUS)
{
	
}

ScottSprite::ScottSprite(int networkId, std::pair<float, float> position, std::string imageFilePath, GraphicsSystems* graphics)
	: GameObject(networkId, position, graphics, imageFilePath, SCOTTSPRITE)
{

}