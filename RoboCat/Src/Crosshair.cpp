#include "RoboCatPCH.h"

Crosshair::Crosshair()
{
	mPosition = Vec2D(100, 100);
	mSize = Vec2D(32, 32);
}

void Crosshair::handleEvent(SDL_Event& e)
{
	int mouseX, mouseY;

	if (e.type == SDL_MOUSEMOTION)
	{
		SDL_GetMouseState(&mouseX, &mouseY);

		mPosition = Vec2D(mouseX, mouseY);
	}
}

void Crosshair::setColor(Color newColor)
{
	mColor = newColor;
}

void Crosshair::update()
{
}

void Crosshair::render()
{
	SDL_SetTextureColorMod(crosshairTexture.getTexture(), mColor.r, mColor.g, mColor.b);
	crosshairTexture.render(mPosition.x, mPosition.y);
}

void Crosshair::WriteBits(OutputMemoryBitStream& inStream)
{
	inStream.Write(Vector3(mPosition.x, mPosition.y, 0));
	inStream.Write(Vector3(mSize.x, mSize.y, 0));
	inStream.Write(Vector3(mColor.r, mColor.g, mColor.b));
}

void Crosshair::ReadBits(InputMemoryBitStream& inStream)
{
	Vector3 position;
	inStream.Read(position);
	mPosition.x = position.mX;
	mPosition.y = position.mY;

	Vector3 size;
	inStream.Read(size);
	mSize.x = size.mX;
	mSize.y = size.mY;

	Vector3 color;
	inStream.Read(color);
	mColor.r = color.mX;
	mColor.g = color.mY;
	mColor.b = color.mZ;
}

Texture Crosshair::crosshairTexture = Texture();