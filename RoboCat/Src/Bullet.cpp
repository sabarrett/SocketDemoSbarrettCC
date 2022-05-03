#include "RoboCatPCH.h"
#include "Bullet.h"

Bullet::Bullet()
{
	mPosition = Vec2D(100, 100);
	mSize = Vec2D(32, 32);
}

void Bullet::setColor(Color newColor)
{
	mColor = newColor;
}

void Bullet::update()
{
	mPosition.x += mVelocity.x;
	mPosition.y += mVelocity.y;
}

void Bullet::render()
{
	SDL_SetTextureColorMod(bulletTexture.getTexture(), mColor.r, mColor.g, mColor.b);
	bulletTexture.render(mPosition.x, mPosition.y);
}

void Bullet::setVelocity(Vec2D newVec2D)
{
	mVelocity = newVec2D;
}

void Bullet::setPosition(Vec2D newPos)
{
	mPosition = newPos;
}

Vec2D Bullet::getPosition()
{
	return mPosition;
}

void Bullet::WriteBits(OutputMemoryBitStream& inStream)
{
	inStream.Write(Vector3(mPosition.x, mPosition.y, 0));
	inStream.Write(Vector3(mSize.x, mSize.y, 0));
	inStream.Write(Vector3(mVelocity.x, mVelocity.y, 0));
	inStream.Write(Vector3(mColor.r, mColor.g, mColor.b));
}

void Bullet::ReadBits(InputMemoryBitStream& inStream)
{
	Vector3 position;
	inStream.Read(position);
	mPosition.x = position.mX;
	mPosition.y = position.mY;

	Vector3 size;
	inStream.Read(size);
	mSize.x = size.mX;
	mSize.y = size.mY;

	Vector3 velocity;
	inStream.Read(velocity);
	mVelocity.x = velocity.mX;
	mVelocity.y = velocity.mY;

	Vector3 color;
	inStream.Read(color);
	mColor.r = color.mX;
	mColor.g = color.mY;
	mColor.b = color.mZ;
}

Texture Bullet::bulletTexture = Texture();