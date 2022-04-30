#include "GraphicsBuffer.h"

GraphicsBuffer::GraphicsBuffer()
{
	mpBitmap = nullptr;
}

GraphicsBuffer::GraphicsBuffer(string ASSET_PATH)
{
	mpBitmap = al_load_bitmap(ASSET_PATH.c_str());
	assert(mpBitmap);
}

GraphicsBuffer::~GraphicsBuffer()
{
}

int GraphicsBuffer::getHeight()
{
	return al_get_bitmap_height(mpBitmap);
}

int GraphicsBuffer::getWidth()
{
	return al_get_bitmap_width(mpBitmap);
}
