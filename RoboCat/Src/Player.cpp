#include "RoboCatPCH.h"

Player::Player()
{
	mPosition = Vec2D(100, 100);
	mSize = Vec2D(32, 32);
	mVelocity = Vec2D(0, 0);
	mAngle = 0.0;
}

void Player::handleEvent(SDL_Event& e)
{
	int mouseX, mouseY;

	if (e.type == SDL_MOUSEMOTION)
	{
		SDL_GetMouseState(&mouseX, &mouseY);

		faceTowards(Vec2D(mouseX, mouseY));
	}
	else if (e.type == SDL_MOUSEBUTTONDOWN)
	{
		double degrees = (mAngle + 180.0) * M_PI / 180;
		mVelocity = Vec2D(
			cos(degrees) / 3.0, sin(degrees) / 3.0
		);
	}
}

void Player::faceTowards(Vec2D pos)
{
	// help from https://gamefromscratch.com/gamedev-math-recipes-rotating-to-face-a-point/
	int centerX = mPosition.x + (mSize.x / 4);
	int centerY = mPosition.y + (mSize.y / 4);
	double angle = atan2(pos.y - centerY, pos.x - centerX);
	angle = angle * (180 / M_PI);
	mAngle = angle;
}

void Player::setColor(Color newColor)
{
	mColor = newColor;
}

Vec2D Player::getPosition()
{
	return mPosition;
}

Vec2D Player::getSize()
{
	return mSize;
}

Vec2D Player::getVelocity()
{
	return mVelocity;
}

void Player::update()
{
	mVelocity.x *= 0.998;
	mVelocity.y *= 0.998;
	
	if (abs(mVelocity.x) < 0.001 && abs(mVelocity.y) < 0.001)
	{
		mVelocity.x = 0.0;
		mVelocity.y = 0.0;
	}
	
	mPosition.x += mVelocity.x;
	mPosition.y += mVelocity.y;
}

void Player::render()
{
	SDL_SetTextureColorMod(playerTexture.getTexture(), mColor.r, mColor.g, mColor.b);
	playerTexture.render(mPosition.x, mPosition.y, NULL, mAngle);
}

void Player::WriteBits(OutputMemoryBitStream& inStream)
{
	inStream.Write(Vector3(mPosition.x, mPosition.y, 0));
	inStream.Write(Vector3(mSize.x, mSize.y, 0));
	inStream.Write(Vector3(mVelocity.x, mVelocity.y, 0));
	inStream.Write(Vector3(mColor.r, mColor.g, mColor.b));
	inStream.Write(mAngle);
}

void Player::ReadBits(InputMemoryBitStream& inStream)
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

	inStream.Read(mAngle);
}

Texture Player::playerTexture = Texture();