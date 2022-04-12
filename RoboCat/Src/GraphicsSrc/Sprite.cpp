#include "Sprite.h"

Sprite::Sprite(const GraphicsBuffer* pBuffer, const Vector2D& srcLoc, int width, int height)
	:mpBuffer(pBuffer)
	,mSrcLoc(srcLoc)
	,mWidth(width)
	,mHeight(height)
{
}

