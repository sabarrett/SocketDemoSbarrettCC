#include "GraphicsBuffer.h"
#include "GraphicsSystem.h"
#include "Color.h"

GraphicsBuffer::GraphicsBuffer(ALLEGRO_DISPLAY* pDisplay)
	:mOwnsBitmap(false)
{
	mpBitmap = al_get_backbuffer(pDisplay);
}

GraphicsBuffer::GraphicsBuffer(const std::string& filename)
	:mOwnsBitmap(true)
{
	mpBitmap = al_load_bitmap(filename.c_str());
	assert(mpBitmap != NULL);
}

GraphicsBuffer::GraphicsBuffer(unsigned int width, unsigned int height, Color color)
	:mOwnsBitmap(true)
{
	mpBitmap = al_create_bitmap(width, height);
	GraphicsSystem::setBufferToColor(*this,color);
	assert(mpBitmap != NULL);
}

GraphicsBuffer::~GraphicsBuffer()
{
	if (mOwnsBitmap)
	{
		al_destroy_bitmap(mpBitmap);
	}
}

GraphicsBuffer * GraphicsBuffer::clone() const
{
	GraphicsBuffer* pNewBuffer = new GraphicsBuffer(getWidth(), getHeight());
	GraphicsSystem::draw(*pNewBuffer, ZERO_VECTOR2D, *this);
	return pNewBuffer;
}

